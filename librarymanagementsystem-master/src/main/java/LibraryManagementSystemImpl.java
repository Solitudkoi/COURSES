import entities.Book;
import entities.Borrow;
import entities.Card;
import queries.*;
import utils.DBInitializer;
import utils.DatabaseConnector;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class LibraryManagementSystemImpl implements LibraryManagementSystem {

    private final DatabaseConnector connector;

    public LibraryManagementSystemImpl(DatabaseConnector connector) {
        this.connector = connector;
    }

    /* ============ 图书管理 ============ */

    @Override
    public ApiResult storeBook(Book book) {
        Connection conn = connector.getConn();
        try {
            // 检查是否已存在相同的书（category, title, press, publish_year, author 全部相同）
            String checkSql = "SELECT book_id FROM book WHERE category = ? AND title = ? " +
                    "AND press = ? AND publish_year = ? AND author = ?";
            PreparedStatement checkStmt = conn.prepareStatement(checkSql);
            checkStmt.setString(1, book.getCategory());
            checkStmt.setString(2, book.getTitle());
            checkStmt.setString(3, book.getPress());
            checkStmt.setInt(4, book.getPublishYear());
            checkStmt.setString(5, book.getAuthor());
            ResultSet rs = checkStmt.executeQuery();
            if (rs.next()) {
                rollback(conn);
                return new ApiResult(false, "Book already exists.");
            }
            // 插入新书
            String insertSql = "INSERT INTO book (category, title, press, publish_year, author, price, stock) " +
                    "VALUES (?, ?, ?, ?, ?, ?, ?)";
            PreparedStatement insertStmt = conn.prepareStatement(insertSql, Statement.RETURN_GENERATED_KEYS);
            insertStmt.setString(1, book.getCategory());
            insertStmt.setString(2, book.getTitle());
            insertStmt.setString(3, book.getPress());
            insertStmt.setInt(4, book.getPublishYear());
            insertStmt.setString(5, book.getAuthor());
            insertStmt.setDouble(6, book.getPrice());
            insertStmt.setInt(7, book.getStock());
            insertStmt.executeUpdate();
            // 获取自增主键并回写到 book 对象
            ResultSet keys = insertStmt.getGeneratedKeys();
            if (keys.next()) {
                book.setBookId(keys.getInt(1));
            }
            commit(conn);
            return new ApiResult(true, null);
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
    }

    @Override
    public ApiResult incBookStock(int bookId, int deltaStock) {
        Connection conn = connector.getConn();
        try {
            // 查询当前库存
            String querySql = "SELECT stock FROM book WHERE book_id = ?";
            PreparedStatement queryStmt = conn.prepareStatement(querySql);
            queryStmt.setInt(1, bookId);
            ResultSet rs = queryStmt.executeQuery();
            if (!rs.next()) {
                rollback(conn);
                return new ApiResult(false, "Book not found.");
            }
            int currentStock = rs.getInt("stock");
            if (currentStock + deltaStock < 0) {
                rollback(conn);
                return new ApiResult(false, "Stock cannot be negative.");
            }
            // 更新库存
            String updateSql = "UPDATE book SET stock = stock + ? WHERE book_id = ?";
            PreparedStatement updateStmt = conn.prepareStatement(updateSql);
            updateStmt.setInt(1, deltaStock);
            updateStmt.setInt(2, bookId);
            updateStmt.executeUpdate();
            commit(conn);
            return new ApiResult(true, null);
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
    }

    @Override
    public ApiResult storeBook(List<Book> books) {
        Connection conn = connector.getConn();
        try {
            String insertSql = "INSERT INTO book (category, title, press, publish_year, author, price, stock) " +
                    "VALUES (?, ?, ?, ?, ?, ?, ?)";
            PreparedStatement insertStmt = conn.prepareStatement(insertSql, Statement.RETURN_GENERATED_KEYS);
            for (Book book : books) {
                insertStmt.setString(1, book.getCategory());
                insertStmt.setString(2, book.getTitle());
                insertStmt.setString(3, book.getPress());
                insertStmt.setInt(4, book.getPublishYear());
                insertStmt.setString(5, book.getAuthor());
                insertStmt.setDouble(6, book.getPrice());
                insertStmt.setInt(7, book.getStock());
                insertStmt.addBatch();
            }
            insertStmt.executeBatch();
            // 获取所有自增主键并回写
            ResultSet keys = insertStmt.getGeneratedKeys();
            int idx = 0;
            while (keys.next()) {
                books.get(idx).setBookId(keys.getInt(1));
                idx++;
            }
            commit(conn);
            return new ApiResult(true, null);
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
    }

    @Override
    public ApiResult removeBook(int bookId) {
        Connection conn = connector.getConn();
        try {
            // 检查是否有未归还的借书记录
            String checkSql = "SELECT COUNT(*) FROM borrow WHERE book_id = ? AND return_time = 0";
            PreparedStatement checkStmt = conn.prepareStatement(checkSql);
            checkStmt.setInt(1, bookId);
            ResultSet rs = checkStmt.executeQuery();
            rs.next();
            if (rs.getInt(1) > 0) {
                rollback(conn);
                return new ApiResult(false, "Book has unreturned borrows.");
            }
            // 删除图书
            String deleteSql = "DELETE FROM book WHERE book_id = ?";
            PreparedStatement deleteStmt = conn.prepareStatement(deleteSql);
            deleteStmt.setInt(1, bookId);
            int affected = deleteStmt.executeUpdate();
            if (affected == 0) {
                rollback(conn);
                return new ApiResult(false, "Book not found.");
            }
            commit(conn);
            return new ApiResult(true, null);
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
    }

    @Override
    public ApiResult modifyBookInfo(Book book) {
        Connection conn = connector.getConn();
        try {
            String updateSql = "UPDATE book SET category = ?, title = ?, press = ?, " +
                    "publish_year = ?, author = ?, price = ? WHERE book_id = ?";
            PreparedStatement updateStmt = conn.prepareStatement(updateSql);
            updateStmt.setString(1, book.getCategory());
            updateStmt.setString(2, book.getTitle());
            updateStmt.setString(3, book.getPress());
            updateStmt.setInt(4, book.getPublishYear());
            updateStmt.setString(5, book.getAuthor());
            updateStmt.setDouble(6, book.getPrice());
            updateStmt.setInt(7, book.getBookId());
            int affected = updateStmt.executeUpdate();
            if (affected == 0) {
                rollback(conn);
                return new ApiResult(false, "Book not found.");
            }
            commit(conn);
            return new ApiResult(true, null);
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
    }

    @Override
    public ApiResult queryBook(BookQueryConditions conditions) {
        Connection conn = connector.getConn();
        try {
            StringBuilder sql = new StringBuilder("SELECT * FROM book WHERE 1=1");
            List<Object> params = new ArrayList<>();

            // 类别：精确查询
            if (conditions.getCategory() != null) {
                sql.append(" AND category = ?");
                params.add(conditions.getCategory());
            }
            // 书名：模糊查询
            if (conditions.getTitle() != null) {
                sql.append(" AND title LIKE ?");
                params.add("%" + conditions.getTitle() + "%");
            }
            // 出版社：模糊查询
            if (conditions.getPress() != null) {
                sql.append(" AND press LIKE ?");
                params.add("%" + conditions.getPress() + "%");
            }
            // 年份范围
            if (conditions.getMinPublishYear() != null) {
                sql.append(" AND publish_year >= ?");
                params.add(conditions.getMinPublishYear());
            }
            if (conditions.getMaxPublishYear() != null) {
                sql.append(" AND publish_year <= ?");
                params.add(conditions.getMaxPublishYear());
            }
            // 作者：模糊查询
            if (conditions.getAuthor() != null) {
                sql.append(" AND author LIKE ?");
                params.add("%" + conditions.getAuthor() + "%");
            }
            // 价格范围
            if (conditions.getMinPrice() != null) {
                sql.append(" AND price >= ?");
                params.add(conditions.getMinPrice());
            }
            if (conditions.getMaxPrice() != null) {
                sql.append(" AND price <= ?");
                params.add(conditions.getMaxPrice());
            }

            // 排序：先按指定列排序，再按 book_id 升序
            sql.append(" ORDER BY ").append(conditions.getSortBy().getValue())
                    .append(" ").append(conditions.getSortOrder().getValue())
                    .append(", book_id ASC");

            PreparedStatement stmt = conn.prepareStatement(sql.toString());
            for (int i = 0; i < params.size(); i++) {
                Object param = params.get(i);
                if (param instanceof String) {
                    stmt.setString(i + 1, (String) param);
                } else if (param instanceof Integer) {
                    stmt.setInt(i + 1, (Integer) param);
                } else if (param instanceof Double) {
                    stmt.setDouble(i + 1, (Double) param);
                }
            }

            ResultSet rs = stmt.executeQuery();
            List<Book> results = new ArrayList<>();
            while (rs.next()) {
                Book b = new Book();
                b.setBookId(rs.getInt("book_id"));
                b.setCategory(rs.getString("category"));
                b.setTitle(rs.getString("title"));
                b.setPress(rs.getString("press"));
                b.setPublishYear(rs.getInt("publish_year"));
                b.setAuthor(rs.getString("author"));
                b.setPrice(rs.getDouble("price"));
                b.setStock(rs.getInt("stock"));
                results.add(b);
            }
            commit(conn);
            return new ApiResult(true, null, new BookQueryResults(results));
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
    }

    /* ============ 借还书 ============ */

    @Override
    public ApiResult borrowBook(Borrow borrow) {
        Connection conn = connector.getConn();
        try {
            // 使用 SELECT ... FOR UPDATE 加行锁，解决并发借书问题（思考题3）
            // 在 InnoDB RR 隔离级别下，普通 SELECT 是快照读，无法感知其他事务的修改
            // FOR UPDATE 将其变为当前读，并对该行加排他锁，其他事务会阻塞等待
            String stockSql = "SELECT stock FROM book WHERE book_id = ? FOR UPDATE";
            PreparedStatement stockStmt = conn.prepareStatement(stockSql);
            stockStmt.setInt(1, borrow.getBookId());
            ResultSet stockRs = stockStmt.executeQuery();
            if (!stockRs.next()) {
                rollback(conn);
                return new ApiResult(false, "Book not found.");
            }
            int stock = stockRs.getInt("stock");
            if (stock <= 0) {
                rollback(conn);
                return new ApiResult(false, "No stock available.");
            }

            // 检查该用户是否已经借了这本书且尚未归还
            String checkSql = "SELECT COUNT(*) FROM borrow WHERE card_id = ? AND book_id = ? AND return_time = 0";
            PreparedStatement checkStmt = conn.prepareStatement(checkSql);
            checkStmt.setInt(1, borrow.getCardId());
            checkStmt.setInt(2, borrow.getBookId());
            ResultSet checkRs = checkStmt.executeQuery();
            checkRs.next();
            if (checkRs.getInt(1) > 0) {
                rollback(conn);
                return new ApiResult(false, "Already borrowed and not returned.");
            }

            // 插入借书记录
            String insertSql = "INSERT INTO borrow (card_id, book_id, borrow_time, return_time) VALUES (?, ?, ?, 0)";
            PreparedStatement insertStmt = conn.prepareStatement(insertSql);
            insertStmt.setInt(1, borrow.getCardId());
            insertStmt.setInt(2, borrow.getBookId());
            insertStmt.setLong(3, borrow.getBorrowTime());
            insertStmt.executeUpdate();

            // 减少库存
            String updateSql = "UPDATE book SET stock = stock - 1 WHERE book_id = ?";
            PreparedStatement updateStmt = conn.prepareStatement(updateSql);
            updateStmt.setInt(1, borrow.getBookId());
            updateStmt.executeUpdate();

            commit(conn);
            return new ApiResult(true, null);
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
    }

    @Override
    public ApiResult returnBook(Borrow borrow) {
        Connection conn = connector.getConn();
        try {
            // 查找对应的未归还借书记录
            String querySql = "SELECT borrow_time FROM borrow WHERE card_id = ? AND book_id = ? AND return_time = 0";
            PreparedStatement queryStmt = conn.prepareStatement(querySql);
            queryStmt.setInt(1, borrow.getCardId());
            queryStmt.setInt(2, borrow.getBookId());
            ResultSet rs = queryStmt.executeQuery();
            if (!rs.next()) {
                rollback(conn);
                return new ApiResult(false, "No matching borrow record found.");
            }
            long borrowTime = rs.getLong("borrow_time");
            // 归还时间必须大于借书时间
            if (borrow.getReturnTime() <= borrowTime) {
                rollback(conn);
                return new ApiResult(false, "Return time must be after borrow time.");
            }

            // 更新归还时间
            String updateSql = "UPDATE borrow SET return_time = ? WHERE card_id = ? AND book_id = ? AND return_time = 0";
            PreparedStatement updateStmt = conn.prepareStatement(updateSql);
            updateStmt.setLong(1, borrow.getReturnTime());
            updateStmt.setInt(2, borrow.getCardId());
            updateStmt.setInt(3, borrow.getBookId());
            updateStmt.executeUpdate();

            // 增加库存
            String stockSql = "UPDATE book SET stock = stock + 1 WHERE book_id = ?";
            PreparedStatement stockStmt = conn.prepareStatement(stockSql);
            stockStmt.setInt(1, borrow.getBookId());
            stockStmt.executeUpdate();

            commit(conn);
            return new ApiResult(true, null);
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
    }

    @Override
    public ApiResult showBorrowHistory(int cardId) {
        Connection conn = connector.getConn();
        try {
            // 联表查询 borrow 和 book，按 borrow_time DESC, book_id ASC 排序
            String sql = "SELECT b.card_id, b.book_id, b.borrow_time, b.return_time, " +
                    "bk.category, bk.title, bk.press, bk.publish_year, bk.author, bk.price " +
                    "FROM borrow b JOIN book bk ON b.book_id = bk.book_id " +
                    "WHERE b.card_id = ? " +
                    "ORDER BY b.borrow_time DESC, b.book_id ASC";
            PreparedStatement stmt = conn.prepareStatement(sql);
            stmt.setInt(1, cardId);
            ResultSet rs = stmt.executeQuery();
            List<BorrowHistories.Item> items = new ArrayList<>();
            while (rs.next()) {
                BorrowHistories.Item item = new BorrowHistories.Item();
                item.setCardId(rs.getInt("card_id"));
                item.setBookId(rs.getInt("book_id"));
                item.setBorrowTime(rs.getLong("borrow_time"));
                item.setReturnTime(rs.getLong("return_time"));
                item.setCategory(rs.getString("category"));
                item.setTitle(rs.getString("title"));
                item.setPress(rs.getString("press"));
                item.setPublishYear(rs.getInt("publish_year"));
                item.setAuthor(rs.getString("author"));
                item.setPrice(rs.getDouble("price"));
                items.add(item);
            }
            commit(conn);
            return new ApiResult(true, null, new BorrowHistories(items));
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
    }

    /* ============ 借书证管理 ============ */

    @Override
    public ApiResult registerCard(Card card) {
        Connection conn = connector.getConn();
        try {
            // 检查是否已存在相同借书证（name, department, type 全部相同）
            String checkSql = "SELECT card_id FROM card WHERE name = ? AND department = ? AND type = ?";
            PreparedStatement checkStmt = conn.prepareStatement(checkSql);
            checkStmt.setString(1, card.getName());
            checkStmt.setString(2, card.getDepartment());
            checkStmt.setString(3, card.getType().getStr());
            ResultSet rs = checkStmt.executeQuery();
            if (rs.next()) {
                rollback(conn);
                return new ApiResult(false, "Card already exists.");
            }
            // 插入新借书证
            String insertSql = "INSERT INTO card (name, department, type) VALUES (?, ?, ?)";
            PreparedStatement insertStmt = conn.prepareStatement(insertSql, Statement.RETURN_GENERATED_KEYS);
            insertStmt.setString(1, card.getName());
            insertStmt.setString(2, card.getDepartment());
            insertStmt.setString(3, card.getType().getStr());
            insertStmt.executeUpdate();
            // 获取自增主键并回写
            ResultSet keys = insertStmt.getGeneratedKeys();
            if (keys.next()) {
                card.setCardId(keys.getInt(1));
            }
            commit(conn);
            return new ApiResult(true, null);
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
    }

    @Override
    public ApiResult removeCard(int cardId) {
        Connection conn = connector.getConn();
        try {
            // 检查是否有未归还的图书
            String checkSql = "SELECT COUNT(*) FROM borrow WHERE card_id = ? AND return_time = 0";
            PreparedStatement checkStmt = conn.prepareStatement(checkSql);
            checkStmt.setInt(1, cardId);
            ResultSet rs = checkStmt.executeQuery();
            rs.next();
            if (rs.getInt(1) > 0) {
                rollback(conn);
                return new ApiResult(false, "Card has unreturned books.");
            }
            // 删除借书证
            String deleteSql = "DELETE FROM card WHERE card_id = ?";
            PreparedStatement deleteStmt = conn.prepareStatement(deleteSql);
            deleteStmt.setInt(1, cardId);
            int affected = deleteStmt.executeUpdate();
            if (affected == 0) {
                rollback(conn);
                return new ApiResult(false, "Card not found.");
            }
            commit(conn);
            return new ApiResult(true, null);
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
    }

    @Override
    public ApiResult showCards() {
        Connection conn = connector.getConn();
        try {
            String sql = "SELECT * FROM card ORDER BY card_id ASC";
            Statement stmt = conn.createStatement();
            ResultSet rs = stmt.executeQuery(sql);
            List<Card> cards = new ArrayList<>();
            while (rs.next()) {
                Card card = new Card();
                card.setCardId(rs.getInt("card_id"));
                card.setName(rs.getString("name"));
                card.setDepartment(rs.getString("department"));
                card.setType(Card.CardType.values(rs.getString("type")));
                cards.add(card);
            }
            commit(conn);
            return new ApiResult(true, null, new CardList(cards));
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
    }

    /* ============ 数据库重置（已提供，不要修改） ============ */

    @Override
    public ApiResult resetDatabase() {
        Connection conn = connector.getConn();
        try {
            Statement stmt = conn.createStatement();
            DBInitializer initializer = connector.getConf().getType().getDbInitializer();
            stmt.addBatch(initializer.sqlDropBorrow());
            stmt.addBatch(initializer.sqlDropBook());
            stmt.addBatch(initializer.sqlDropCard());
            stmt.addBatch(initializer.sqlCreateCard());
            stmt.addBatch(initializer.sqlCreateBook());
            stmt.addBatch(initializer.sqlCreateBorrow());
            stmt.executeBatch();
            commit(conn);
        } catch (Exception e) {
            rollback(conn);
            return new ApiResult(false, e.getMessage());
        }
        return new ApiResult(true, null);
    }

    private void rollback(Connection conn) {
        try {
            conn.rollback();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void commit(Connection conn) {
        try {
            conn.commit();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
