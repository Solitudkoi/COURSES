import com.sun.net.httpserver.Headers;
import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;
import entities.Book;
import entities.Borrow;
import entities.Card;
import queries.*;
import utils.ConnectConfig;
import utils.DatabaseConnector;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.URLDecoder;
import java.util.*;
import java.util.logging.Logger;

public class Main {

    private static final Logger log = Logger.getLogger(Main.class.getName());
    private static LibraryManagementSystem library;
    private static DatabaseConnector connector;

    public static void main(String[] args) {
        try {
            // 连接数据库
            ConnectConfig conf = new ConnectConfig();
            log.info("Success to parse connect config. " + conf.toString());
            connector = new DatabaseConnector(conf);
            boolean connStatus = connector.connect();
            if (!connStatus) {
                log.severe("Failed to connect database.");
                System.exit(1);
            }
            library = new LibraryManagementSystemImpl(connector);

            // 创建 HTTP 服务器
            HttpServer server = HttpServer.create(new InetSocketAddress(8000), 0);
            server.createContext("/card", new CardHandler());
            server.createContext("/book", new BookHandler());
            server.createContext("/borrow", new BorrowHandler());
            server.start();

            System.out.println("Server is listening on port 8000");

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // ==================== 工具方法 ====================

    /** 为响应添加跨域头 */
    private static void addCorsHeaders(HttpExchange exchange) {
        Headers headers = exchange.getResponseHeaders();
        headers.add("Access-Control-Allow-Origin", "*");
        headers.add("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        headers.add("Access-Control-Allow-Headers", "Content-Type");
    }

    /** 读取请求体为字符串 */
    private static String readRequestBody(HttpExchange exchange) throws IOException {
        InputStream is = exchange.getRequestBody();
        BufferedReader reader = new BufferedReader(new InputStreamReader(is, "UTF-8"));
        StringBuilder sb = new StringBuilder();
        String line;
        while ((line = reader.readLine()) != null) {
            sb.append(line);
        }
        reader.close();
        return sb.toString();
    }

    /** 发送 JSON 响应 */
    private static void sendResponse(HttpExchange exchange, int statusCode, String json) throws IOException {
        exchange.getResponseHeaders().set("Content-Type", "application/json; charset=UTF-8");
        byte[] bytes = json.getBytes("UTF-8");
        exchange.sendResponseHeaders(statusCode, bytes.length);
        OutputStream os = exchange.getResponseBody();
        os.write(bytes);
        os.close();
    }

    /** 处理 OPTIONS 预检请求 */
    private static void handleOptions(HttpExchange exchange) throws IOException {
        exchange.sendResponseHeaders(204, -1);
    }

    /** 解析 URL 中的查询参数 */
    private static Map<String, String> parseQueryParams(String query) {
        Map<String, String> params = new HashMap<>();
        if (query == null || query.isEmpty()) return params;
        for (String pair : query.split("&")) {
            String[] kv = pair.split("=", 2);
            try {
                String key = URLDecoder.decode(kv[0], "UTF-8");
                String value = kv.length > 1 ? URLDecoder.decode(kv[1], "UTF-8") : "";
                params.put(key, value);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return params;
    }

    // ==================== 简易 JSON 解析（不依赖第三方库） ====================

    /** 从 JSON 字符串中提取 string 值 */
    private static String jsonGetString(String json, String key) {
        String search = "\"" + key + "\"";
        int idx = json.indexOf(search);
        if (idx == -1) return null;
        idx = json.indexOf(":", idx + search.length());
        if (idx == -1) return null;
        // 跳过空格
        idx++;
        while (idx < json.length() && json.charAt(idx) == ' ') idx++;
        if (idx >= json.length()) return null;
        if (json.charAt(idx) == '"') {
            // 字符串值
            int start = idx + 1;
            int end = json.indexOf("\"", start);
            return json.substring(start, end);
        }
        return null;
    }

    /** 从 JSON 字符串中提取 int 值 */
    private static int jsonGetInt(String json, String key) {
        String search = "\"" + key + "\"";
        int idx = json.indexOf(search);
        if (idx == -1) return 0;
        idx = json.indexOf(":", idx + search.length());
        if (idx == -1) return 0;
        idx++;
        while (idx < json.length() && json.charAt(idx) == ' ') idx++;
        StringBuilder sb = new StringBuilder();
        while (idx < json.length() && (Character.isDigit(json.charAt(idx)) || json.charAt(idx) == '-')) {
            sb.append(json.charAt(idx));
            idx++;
        }
        return Integer.parseInt(sb.toString());
    }

    /** 从 JSON 字符串中提取 long 值 */
    private static long jsonGetLong(String json, String key) {
        String search = "\"" + key + "\"";
        int idx = json.indexOf(search);
        if (idx == -1) return 0;
        idx = json.indexOf(":", idx + search.length());
        if (idx == -1) return 0;
        idx++;
        while (idx < json.length() && json.charAt(idx) == ' ') idx++;
        StringBuilder sb = new StringBuilder();
        while (idx < json.length() && (Character.isDigit(json.charAt(idx)) || json.charAt(idx) == '-')) {
            sb.append(json.charAt(idx));
            idx++;
        }
        return Long.parseLong(sb.toString());
    }

    /** 从 JSON 字符串中提取 double 值 */
    private static double jsonGetDouble(String json, String key) {
        String search = "\"" + key + "\"";
        int idx = json.indexOf(search);
        if (idx == -1) return 0;
        idx = json.indexOf(":", idx + search.length());
        if (idx == -1) return 0;
        idx++;
        while (idx < json.length() && json.charAt(idx) == ' ') idx++;
        StringBuilder sb = new StringBuilder();
        while (idx < json.length() && (Character.isDigit(json.charAt(idx)) || json.charAt(idx) == '.' || json.charAt(idx) == '-')) {
            sb.append(json.charAt(idx));
            idx++;
        }
        return Double.parseDouble(sb.toString());
    }

    /** 转义 JSON 字符串中的特殊字符 */
    private static String escapeJson(String s) {
        if (s == null) return "";
        return s.replace("\\", "\\\\").replace("\"", "\\\"")
                .replace("\n", "\\n").replace("\r", "\\r").replace("\t", "\\t");
    }

    // ==================== CardHandler ====================

    static class CardHandler implements HttpHandler {
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            addCorsHeaders(exchange);
            String method = exchange.getRequestMethod();
            if ("GET".equals(method)) {
                handleGet(exchange);
            } else if ("POST".equals(method)) {
                handlePost(exchange);
            } else if ("DELETE".equals(method)) {
                handleDelete(exchange);
            } else if ("OPTIONS".equals(method)) {
                handleOptions(exchange);
            } else {
                exchange.sendResponseHeaders(405, -1);
            }
        }

        private void handleGet(HttpExchange exchange) throws IOException {
            ApiResult result = library.showCards();
            if (!result.ok) {
                sendResponse(exchange, 500, "{\"ok\":false,\"message\":\"" + escapeJson(result.message) + "\"}");
                return;
            }
            CardList cardList = (CardList) result.payload;
            StringBuilder sb = new StringBuilder("[");
            List<Card> cards = cardList.getCards();
            for (int i = 0; i < cards.size(); i++) {
                Card c = cards.get(i);
                String typeStr = c.getType() == Card.CardType.Teacher ? "教师" : "学生";
                sb.append("{");
                sb.append("\"id\":").append(c.getCardId()).append(",");
                sb.append("\"name\":\"").append(escapeJson(c.getName())).append("\",");
                sb.append("\"department\":\"").append(escapeJson(c.getDepartment())).append("\",");
                sb.append("\"type\":\"").append(typeStr).append("\"");
                sb.append("}");
                if (i < cards.size() - 1) sb.append(",");
            }
            sb.append("]");
            sendResponse(exchange, 200, sb.toString());
        }

        private void handlePost(HttpExchange exchange) throws IOException {
            String body = readRequestBody(exchange);
            String name = jsonGetString(body, "name");
            String department = jsonGetString(body, "department");
            String typeStr = jsonGetString(body, "type");
            // 前端传来 "教师"/"学生"，转换为 CardType
            Card.CardType type;
            if ("教师".equals(typeStr) || "T".equals(typeStr)) {
                type = Card.CardType.Teacher;
            } else {
                type = Card.CardType.Student;
            }
            Card card = new Card(0, name, department, type);
            ApiResult result = library.registerCard(card);
            if (result.ok) {
                sendResponse(exchange, 200, "{\"ok\":true,\"cardId\":" + card.getCardId() + "}");
            } else {
                sendResponse(exchange, 400, "{\"ok\":false,\"message\":\"" + escapeJson(result.message) + "\"}");
            }
        }

        private void handleDelete(HttpExchange exchange) throws IOException {
            Map<String, String> params = parseQueryParams(exchange.getRequestURI().getQuery());
            String idStr = params.get("id");
            if (idStr == null) {
                // 也尝试从请求体读取
                String body = readRequestBody(exchange);
                if (body != null && !body.isEmpty()) {
                    idStr = String.valueOf(jsonGetInt(body, "id"));
                }
            }
            int cardId = Integer.parseInt(idStr);
            ApiResult result = library.removeCard(cardId);
            if (result.ok) {
                sendResponse(exchange, 200, "{\"ok\":true}");
            } else {
                sendResponse(exchange, 400, "{\"ok\":false,\"message\":\"" + escapeJson(result.message) + "\"}");
            }
        }
    }

    // ==================== BorrowHandler ====================

    static class BorrowHandler implements HttpHandler {
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            addCorsHeaders(exchange);
            String method = exchange.getRequestMethod();
            if ("GET".equals(method)) {
                handleGet(exchange);
            } else if ("OPTIONS".equals(method)) {
                handleOptions(exchange);
            } else {
                exchange.sendResponseHeaders(405, -1);
            }
        }

        private void handleGet(HttpExchange exchange) throws IOException {
            Map<String, String> params = parseQueryParams(exchange.getRequestURI().getQuery());
            String cardIdStr = params.get("cardID");  // 老版前端用 cardID
            if (cardIdStr == null) {
                cardIdStr = params.get("cardId");  // 兼容新版前端
            }
            if (cardIdStr == null || cardIdStr.isEmpty()) {
                sendResponse(exchange, 400, "[]");
                return;
            }
            int cardId = Integer.parseInt(cardIdStr);
            ApiResult result = library.showBorrowHistory(cardId);
            if (!result.ok) {
                sendResponse(exchange, 500, "[]");
                return;
            }
            BorrowHistories histories = (BorrowHistories) result.payload;
            StringBuilder sb = new StringBuilder("[");
            List<BorrowHistories.Item> items = histories.getItems();
            for (int i = 0; i < items.size(); i++) {
                BorrowHistories.Item item = items.get(i);
                sb.append("{");
                sb.append("\"cardID\":").append(item.getCardId()).append(",");
                sb.append("\"bookID\":").append(item.getBookId()).append(",");
                sb.append("\"borrowTime\":").append(item.getBorrowTime()).append(",");
                sb.append("\"returnTime\":").append(item.getReturnTime());
                sb.append("}");
                if (i < items.size() - 1) sb.append(",");
            }
            sb.append("]");
            sendResponse(exchange, 200, sb.toString());
        }
    }

    // ==================== BookHandler ====================

    static class BookHandler implements HttpHandler {
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            addCorsHeaders(exchange);
            String method = exchange.getRequestMethod();
            String path = exchange.getRequestURI().getPath();

            if ("OPTIONS".equals(method)) {
                handleOptions(exchange);
                return;
            }

            // 子路由
            if (path.equals("/book/borrow") && "POST".equals(method)) {
                handleBorrow(exchange);
            } else if (path.equals("/book/return") && "POST".equals(method)) {
                handleReturn(exchange);
            } else if (path.equals("/book/stock") && "POST".equals(method)) {
                handleStock(exchange);
            } else if (path.equals("/book/batch") && "POST".equals(method)) {
                handleBatch(exchange);
            } else if (path.equals("/book")) {
                if ("GET".equals(method)) {
                    handleQuery(exchange);
                } else if ("POST".equals(method)) {
                    handleStore(exchange);
                } else if ("PUT".equals(method)) {
                    handleModify(exchange);
                } else if ("DELETE".equals(method)) {
                    handleRemove(exchange);
                } else {
                    exchange.sendResponseHeaders(405, -1);
                }
            } else {
                exchange.sendResponseHeaders(404, -1);
            }
        }

        /** GET /book?category=...&title=...&... 查询图书 */
        private void handleQuery(HttpExchange exchange) throws IOException {
            Map<String, String> params = parseQueryParams(exchange.getRequestURI().getQuery());
            BookQueryConditions conditions = new BookQueryConditions();
            if (params.containsKey("category") && !params.get("category").isEmpty())
                conditions.setCategory(params.get("category"));
            if (params.containsKey("title") && !params.get("title").isEmpty())
                conditions.setTitle(params.get("title"));
            if (params.containsKey("press") && !params.get("press").isEmpty())
                conditions.setPress(params.get("press"));
            if (params.containsKey("author") && !params.get("author").isEmpty())
                conditions.setAuthor(params.get("author"));
            if (params.containsKey("minPublishYear") && !params.get("minPublishYear").isEmpty())
                conditions.setMinPublishYear(Integer.parseInt(params.get("minPublishYear")));
            if (params.containsKey("maxPublishYear") && !params.get("maxPublishYear").isEmpty())
                conditions.setMaxPublishYear(Integer.parseInt(params.get("maxPublishYear")));
            if (params.containsKey("minPrice") && !params.get("minPrice").isEmpty())
                conditions.setMinPrice(Double.parseDouble(params.get("minPrice")));
            if (params.containsKey("maxPrice") && !params.get("maxPrice").isEmpty())
                conditions.setMaxPrice(Double.parseDouble(params.get("maxPrice")));
            if (params.containsKey("sortBy") && !params.get("sortBy").isEmpty()) {
                try {
                    conditions.setSortBy(Book.SortColumn.valueOf(params.get("sortBy")));
                } catch (Exception e) { /* 忽略非法排序列 */ }
            }
            if (params.containsKey("sortOrder") && !params.get("sortOrder").isEmpty()) {
                conditions.setSortOrder("desc".equalsIgnoreCase(params.get("sortOrder")) ? SortOrder.DESC : SortOrder.ASC);
            }

            ApiResult result = library.queryBook(conditions);
            if (!result.ok) {
                sendResponse(exchange, 500, "{\"ok\":false,\"message\":\"" + escapeJson(result.message) + "\"}");
                return;
            }
            BookQueryResults results = (BookQueryResults) result.payload;
            StringBuilder sb = new StringBuilder("[");
            List<Book> books = results.getResults();
            for (int i = 0; i < books.size(); i++) {
                Book b = books.get(i);
                sb.append("{");
                sb.append("\"bookId\":").append(b.getBookId()).append(",");
                sb.append("\"category\":\"").append(escapeJson(b.getCategory())).append("\",");
                sb.append("\"title\":\"").append(escapeJson(b.getTitle())).append("\",");
                sb.append("\"press\":\"").append(escapeJson(b.getPress())).append("\",");
                sb.append("\"publishYear\":").append(b.getPublishYear()).append(",");
                sb.append("\"author\":\"").append(escapeJson(b.getAuthor())).append("\",");
                sb.append("\"price\":").append(String.format("%.2f", b.getPrice())).append(",");
                sb.append("\"stock\":").append(b.getStock());
                sb.append("}");
                if (i < books.size() - 1) sb.append(",");
            }
            sb.append("]");
            sendResponse(exchange, 200, sb.toString());
        }

        /** POST /book 入库单本图书 */
        private void handleStore(HttpExchange exchange) throws IOException {
            String body = readRequestBody(exchange);
            Book book = new Book();
            book.setCategory(jsonGetString(body, "category"));
            book.setTitle(jsonGetString(body, "title"));
            book.setPress(jsonGetString(body, "press"));
            book.setPublishYear(jsonGetInt(body, "publishYear"));
            book.setAuthor(jsonGetString(body, "author"));
            book.setPrice(jsonGetDouble(body, "price"));
            book.setStock(jsonGetInt(body, "stock"));

            ApiResult result = library.storeBook(book);
            if (result.ok) {
                sendResponse(exchange, 200, "{\"ok\":true,\"bookId\":" + book.getBookId() + "}");
            } else {
                sendResponse(exchange, 400, "{\"ok\":false,\"message\":\"" + escapeJson(result.message) + "\"}");
            }
        }

        /** PUT /book 修改图书信息 */
        private void handleModify(HttpExchange exchange) throws IOException {
            String body = readRequestBody(exchange);
            Book book = new Book();
            book.setBookId(jsonGetInt(body, "bookId"));
            book.setCategory(jsonGetString(body, "category"));
            book.setTitle(jsonGetString(body, "title"));
            book.setPress(jsonGetString(body, "press"));
            book.setPublishYear(jsonGetInt(body, "publishYear"));
            book.setAuthor(jsonGetString(body, "author"));
            book.setPrice(jsonGetDouble(body, "price"));

            ApiResult result = library.modifyBookInfo(book);
            if (result.ok) {
                sendResponse(exchange, 200, "{\"ok\":true}");
            } else {
                sendResponse(exchange, 400, "{\"ok\":false,\"message\":\"" + escapeJson(result.message) + "\"}");
            }
        }

        /** DELETE /book?bookId=... 删除图书 */
        private void handleRemove(HttpExchange exchange) throws IOException {
            Map<String, String> params = parseQueryParams(exchange.getRequestURI().getQuery());
            int bookId = Integer.parseInt(params.get("bookId"));
            ApiResult result = library.removeBook(bookId);
            if (result.ok) {
                sendResponse(exchange, 200, "{\"ok\":true}");
            } else {
                sendResponse(exchange, 400, "{\"ok\":false,\"message\":\"" + escapeJson(result.message) + "\"}");
            }
        }

        /** POST /book/borrow 借书 */
        private void handleBorrow(HttpExchange exchange) throws IOException {
            String body = readRequestBody(exchange);
            Borrow borrow = new Borrow();
            borrow.setCardId(jsonGetInt(body, "cardId"));
            borrow.setBookId(jsonGetInt(body, "bookId"));
            borrow.resetBorrowTime();

            ApiResult result = library.borrowBook(borrow);
            if (result.ok) {
                sendResponse(exchange, 200, "{\"ok\":true}");
            } else {
                sendResponse(exchange, 400, "{\"ok\":false,\"message\":\"" + escapeJson(result.message) + "\"}");
            }
        }

        /** POST /book/return 还书 */
        private void handleReturn(HttpExchange exchange) throws IOException {
            String body = readRequestBody(exchange);
            Borrow borrow = new Borrow();
            borrow.setCardId(jsonGetInt(body, "cardId"));
            borrow.setBookId(jsonGetInt(body, "bookId"));
            borrow.resetReturnTime();

            ApiResult result = library.returnBook(borrow);
            if (result.ok) {
                sendResponse(exchange, 200, "{\"ok\":true}");
            } else {
                sendResponse(exchange, 400, "{\"ok\":false,\"message\":\"" + escapeJson(result.message) + "\"}");
            }
        }

        /** POST /book/stock 增减库存 */
        private void handleStock(HttpExchange exchange) throws IOException {
            String body = readRequestBody(exchange);
            int bookId = jsonGetInt(body, "bookId");
            int deltaStock = jsonGetInt(body, "deltaStock");

            ApiResult result = library.incBookStock(bookId, deltaStock);
            if (result.ok) {
                sendResponse(exchange, 200, "{\"ok\":true}");
            } else {
                sendResponse(exchange, 400, "{\"ok\":false,\"message\":\"" + escapeJson(result.message) + "\"}");
            }
        }

        /** POST /book/batch 批量入库（接收 CSV 格式的文本） */
        private void handleBatch(HttpExchange exchange) throws IOException {
            String body = readRequestBody(exchange);
            // 解析 JSON 数组格式的批量图书数据
            // 格式: [{"category":"...", "title":"...", ...}, ...]
            List<Book> books = new ArrayList<>();
            // 简单解析：按 "},{" 拆分
            String trimmed = body.trim();
            if (trimmed.startsWith("[")) trimmed = trimmed.substring(1);
            if (trimmed.endsWith("]")) trimmed = trimmed.substring(0, trimmed.length() - 1);
            String[] items = trimmed.split("\\},\\s*\\{");
            for (String item : items) {
                String s = item.trim();
                if (!s.startsWith("{")) s = "{" + s;
                if (!s.endsWith("}")) s = s + "}";
                Book book = new Book();
                book.setCategory(jsonGetString(s, "category"));
                book.setTitle(jsonGetString(s, "title"));
                book.setPress(jsonGetString(s, "press"));
                book.setPublishYear(jsonGetInt(s, "publishYear"));
                book.setAuthor(jsonGetString(s, "author"));
                book.setPrice(jsonGetDouble(s, "price"));
                book.setStock(jsonGetInt(s, "stock"));
                books.add(book);
            }

            ApiResult result = library.storeBook(books);
            if (result.ok) {
                sendResponse(exchange, 200, "{\"ok\":true,\"count\":" + books.size() + "}");
            } else {
                sendResponse(exchange, 400, "{\"ok\":false,\"message\":\"" + escapeJson(result.message) + "\"}");
            }
        }
    }
}