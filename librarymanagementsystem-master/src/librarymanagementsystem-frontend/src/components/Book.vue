<template>
    <el-scrollbar height="100%" style="width: 100%;">
        <div style="margin-top: 20px; margin-left: 40px; font-size: 2em; font-weight: bold;">图书管理</div>

        <!-- 操作按钮区 -->
        <div style="margin: 20px 40px;">
            <el-button type="primary" @click="storeDialogVisible = true">图书入库</el-button>
            <el-button type="success" @click="batchDialogVisible = true">批量入库</el-button>
            <el-button type="warning" @click="borrowDialogVisible = true">借书</el-button>
            <el-button type="info" @click="returnDialogVisible = true">还书</el-button>
        </div>

        <!-- 查询条件区 -->
        <div style="margin: 10px 40px; display: flex; flex-wrap: wrap; gap: 10px; align-items: center;">
            <el-input v-model="query.category" placeholder="类别(精确)" style="width: 130px;" clearable />
            <el-input v-model="query.title" placeholder="书名(模糊)" style="width: 130px;" clearable />
            <el-input v-model="query.press" placeholder="出版社(模糊)" style="width: 130px;" clearable />
            <el-input v-model="query.author" placeholder="作者(模糊)" style="width: 130px;" clearable />
            <el-input v-model="query.minPublishYear" placeholder="最小年份" style="width: 100px;" clearable />
            <el-input v-model="query.maxPublishYear" placeholder="最大年份" style="width: 100px;" clearable />
            <el-input v-model="query.minPrice" placeholder="最低价" style="width: 90px;" clearable />
            <el-input v-model="query.maxPrice" placeholder="最高价" style="width: 90px;" clearable />
            <el-button type="primary" @click="QueryBooks">查询</el-button>
            <el-button @click="ResetQuery">重置</el-button>
        </div>

        <!-- 图书列表 -->
        <el-table :data="books" style="width: 95%; margin: 20px 40px;" :table-layout="'auto'" height="500"
            stripe border>
            <el-table-column prop="bookId" label="ID" width="60" sortable />
            <el-table-column prop="category" label="类别" width="100" />
            <el-table-column prop="title" label="书名" min-width="150" />
            <el-table-column prop="press" label="出版社" min-width="120" />
            <el-table-column prop="publishYear" label="年份" width="80" sortable />
            <el-table-column prop="author" label="作者" width="100" />
            <el-table-column prop="price" label="价格" width="80" sortable />
            <el-table-column prop="stock" label="库存" width="80" sortable />
            <el-table-column label="操作" width="220" fixed="right">
                <template #default="scope">
                    <el-button size="small" @click="openModify(scope.row)">编辑</el-button>
                    <el-button size="small" type="warning"
                        @click="stockInfo.bookId = scope.row.bookId, stockDialogVisible = true">库存</el-button>
                    <el-button size="small" type="danger" @click="RemoveBook(scope.row.bookId)">删除</el-button>
                </template>
            </el-table-column>
        </el-table>

        <!-- ======== 图书入库对话框 ======== -->
        <el-dialog v-model="storeDialogVisible" title="图书入库" width="35%" align-center>
            <div class="form-item">类别：<el-input v-model="storeInfo.category" clearable /></div>
            <div class="form-item">书名：<el-input v-model="storeInfo.title" clearable /></div>
            <div class="form-item">出版社：<el-input v-model="storeInfo.press" clearable /></div>
            <div class="form-item">年份：<el-input v-model.number="storeInfo.publishYear" clearable /></div>
            <div class="form-item">作者：<el-input v-model="storeInfo.author" clearable /></div>
            <div class="form-item">价格：<el-input v-model.number="storeInfo.price" clearable /></div>
            <div class="form-item">库存：<el-input v-model.number="storeInfo.stock" clearable /></div>
            <template #footer>
                <el-button @click="storeDialogVisible = false">取消</el-button>
                <el-button type="primary" @click="StoreBook">确定</el-button>
            </template>
        </el-dialog>

        <!-- ======== 修改图书对话框 ======== -->
        <el-dialog v-model="modifyDialogVisible" title="修改图书信息" width="35%" align-center>
            <div class="form-item">ID：<el-input v-model="modifyInfo.bookId" disabled /></div>
            <div class="form-item">类别：<el-input v-model="modifyInfo.category" clearable /></div>
            <div class="form-item">书名：<el-input v-model="modifyInfo.title" clearable /></div>
            <div class="form-item">出版社：<el-input v-model="modifyInfo.press" clearable /></div>
            <div class="form-item">年份：<el-input v-model.number="modifyInfo.publishYear" clearable /></div>
            <div class="form-item">作者：<el-input v-model="modifyInfo.author" clearable /></div>
            <div class="form-item">价格：<el-input v-model.number="modifyInfo.price" clearable /></div>
            <template #footer>
                <el-button @click="modifyDialogVisible = false">取消</el-button>
                <el-button type="primary" @click="ModifyBook">确定</el-button>
            </template>
        </el-dialog>

        <!-- ======== 增减库存对话框 ======== -->
        <el-dialog v-model="stockDialogVisible" title="增减库存" width="30%" align-center>
            <div class="form-item">图书ID：<el-input v-model="stockInfo.bookId" disabled /></div>
            <div class="form-item">增量（负数为减少）：<el-input v-model.number="stockInfo.deltaStock" clearable /></div>
            <template #footer>
                <el-button @click="stockDialogVisible = false">取消</el-button>
                <el-button type="primary" @click="IncStock">确定</el-button>
            </template>
        </el-dialog>

        <!-- ======== 借书对话框 ======== -->
        <el-dialog v-model="borrowDialogVisible" title="借书" width="30%" align-center>
            <div class="form-item">借书证ID：<el-input v-model.number="borrowInfo.cardId" clearable /></div>
            <div class="form-item">图书ID：<el-input v-model.number="borrowInfo.bookId" clearable /></div>
            <template #footer>
                <el-button @click="borrowDialogVisible = false">取消</el-button>
                <el-button type="primary" @click="BorrowBook">确定</el-button>
            </template>
        </el-dialog>

        <!-- ======== 还书对话框 ======== -->
        <el-dialog v-model="returnDialogVisible" title="还书" width="30%" align-center>
            <div class="form-item">借书证ID：<el-input v-model.number="returnInfo.cardId" clearable /></div>
            <div class="form-item">图书ID：<el-input v-model.number="returnInfo.bookId" clearable /></div>
            <template #footer>
                <el-button @click="returnDialogVisible = false">取消</el-button>
                <el-button type="primary" @click="ReturnBook">确定</el-button>
            </template>
        </el-dialog>

        <!-- ======== 批量入库对话框 ======== -->
        <el-dialog v-model="batchDialogVisible" title="批量入库" width="50%" align-center>
            <p style="margin-bottom: 10px; color: #666;">每行一本书，格式：类别,书名,出版社,年份,作者,价格,库存</p>
            <el-input v-model="batchText" type="textarea" :rows="10"
                placeholder="例如：&#10;计算机,数据库系统概论,高等教育出版社,2014,王珊,39.00,10&#10;文学,红楼梦,人民文学出版社,1996,曹雪芹,59.70,5" />
            <template #footer>
                <el-button @click="batchDialogVisible = false">取消</el-button>
                <el-button type="primary" @click="BatchStore">导入</el-button>
            </template>
        </el-dialog>

    </el-scrollbar>
</template>

<script>
import { ElMessage, ElMessageBox } from 'element-plus'
import axios from 'axios'

export default {
    data() {
        return {
            books: [],
            // 查询条件
            query: {
                category: '', title: '', press: '', author: '',
                minPublishYear: '', maxPublishYear: '',
                minPrice: '', maxPrice: ''
            },
            // 入库
            storeDialogVisible: false,
            storeInfo: { category: '', title: '', press: '', publishYear: 2024, author: '', price: 0, stock: 0 },
            // 修改
            modifyDialogVisible: false,
            modifyInfo: { bookId: 0, category: '', title: '', press: '', publishYear: 0, author: '', price: 0 },
            // 库存
            stockDialogVisible: false,
            stockInfo: { bookId: 0, deltaStock: 0 },
            // 借书
            borrowDialogVisible: false,
            borrowInfo: { cardId: '', bookId: '' },
            // 还书
            returnDialogVisible: false,
            returnInfo: { cardId: '', bookId: '' },
            // 批量入库
            batchDialogVisible: false,
            batchText: ''
        }
    },
    methods: {
        /** 查询图书 */
        QueryBooks() {
            let params = {}
            if (this.query.category) params.category = this.query.category
            if (this.query.title) params.title = this.query.title
            if (this.query.press) params.press = this.query.press
            if (this.query.author) params.author = this.query.author
            if (this.query.minPublishYear) params.minPublishYear = this.query.minPublishYear
            if (this.query.maxPublishYear) params.maxPublishYear = this.query.maxPublishYear
            if (this.query.minPrice) params.minPrice = this.query.minPrice
            if (this.query.maxPrice) params.maxPrice = this.query.maxPrice

            axios.get('/book', { params })
                .then(response => {
                    this.books = response.data
                })
                .catch(() => {
                    ElMessage.error("查询失败")
                })
        },
        /** 重置查询条件 */
        ResetQuery() {
            this.query = {
                category: '', title: '', press: '', author: '',
                minPublishYear: '', maxPublishYear: '',
                minPrice: '', maxPrice: ''
            }
            this.QueryBooks()
        },
        /** 图书入库 */
        StoreBook() {
            if (this.storeInfo.price < 0) {
                ElMessage.error("价格不能为负数")
                return
            }
            if (this.storeInfo.stock < 0) {
                ElMessage.error("库存不能为负数")
                return
            }
            axios.post('/book', this.storeInfo)
                .then(() => {
                    ElMessage.success("入库成功")
                    this.storeDialogVisible = false
                    this.storeInfo = { category: '', title: '', press: '', publishYear: 2024, author: '', price: 0, stock: 0 }
                    this.QueryBooks()
                })
                .catch(error => {
                    ElMessage.error("入库失败：" + (error.response?.data?.message || "未知错误"))
                })
        },
        /** 打开修改对话框 */
        openModify(row) {
            this.modifyInfo = {
                bookId: row.bookId,
                category: row.category,
                title: row.title,
                press: row.press,
                publishYear: row.publishYear,
                author: row.author,
                price: row.price
            }
            this.modifyDialogVisible = true
        },
        /** 修改图书 */
        ModifyBook() {
            if (this.modifyInfo.price < 0) {
                ElMessage.error("价格不能为负数")
                return
            }
            axios.put('/book', this.modifyInfo)
                .then(() => {
                    ElMessage.success("修改成功")
                    this.modifyDialogVisible = false
                    this.QueryBooks()
                })
                .catch(error => {
                    ElMessage.error("修改失败：" + (error.response?.data?.message || "未知错误"))
                })
        },
        /** 删除图书 */
        RemoveBook(bookId) {
            ElMessageBox.confirm('确定删除该图书吗？', '提示', { type: 'warning' })
                .then(() => {
                    axios.delete('/book', { params: { bookId: bookId } })
                        .then(() => {
                            ElMessage.success("删除成功")
                            this.QueryBooks()
                        })
                        .catch(error => {
                            ElMessage.error("删除失败：" + (error.response?.data?.message || "该图书还有未归还记录"))
                        })
                })
                .catch(() => { })
        },
        /** 增减库存 */
        IncStock() {
            axios.post('/book/stock', this.stockInfo)
                .then(() => {
                    ElMessage.success("库存更新成功")
                    this.stockDialogVisible = false
                    this.stockInfo.deltaStock = 0
                    this.QueryBooks()
                })
                .catch(error => {
                    ElMessage.error("更新失败：" + (error.response?.data?.message || "库存不能为负"))
                })
        },
        /** 借书 */
        BorrowBook() {
            axios.post('/book/borrow', this.borrowInfo)
                .then(() => {
                    ElMessage.success("借书成功")
                    this.borrowDialogVisible = false
                    this.borrowInfo = { cardId: '', bookId: '' }
                    this.QueryBooks()
                })
                .catch(error => {
                    ElMessage.error("借书失败：" + (error.response?.data?.message || "未知错误"))
                })
        },
        /** 还书 */
        ReturnBook() {
            axios.post('/book/return', this.returnInfo)
                .then(() => {
                    ElMessage.success("还书成功")
                    this.returnDialogVisible = false
                    this.returnInfo = { cardId: '', bookId: '' }
                    this.QueryBooks()
                })
                .catch(error => {
                    ElMessage.error("还书失败：" + (error.response?.data?.message || "未知错误"))
                })
        },
        /** 批量入库 */
        BatchStore() {
            let lines = this.batchText.trim().split('\n')
            let books = []
            for (let line of lines) {
                let parts = line.split(',')
                if (parts.length < 7) {
                    ElMessage.error("格式错误，请检查：" + line)
                    return
                }
                let price = parseFloat(parts[5].trim())
                let stock = parseInt(parts[6].trim())
                if (price < 0) {
                    ElMessage.error("价格不能为负数：" + line)
                    return
                }
                if (stock < 0) {
                    ElMessage.error("库存不能为负数：" + line)
                    return
                }
                books.push({
                    category: parts[0].trim(),
                    title: parts[1].trim(),
                    press: parts[2].trim(),
                    publishYear: parseInt(parts[3].trim()),
                    author: parts[4].trim(),
                    price: parseFloat(parts[5].trim()),
                    stock: parseInt(parts[6].trim())
                })
            }
            axios.post('/book/batch', books)
                .then(() => {
                    ElMessage.success("批量入库成功，共 " + books.length + " 本")
                    this.batchDialogVisible = false
                    this.batchText = ''
                    this.QueryBooks()
                })
                .catch(error => {
                    ElMessage.error("批量入库失败：" + (error.response?.data?.message || "有重复图书或格式错误"))
                })
        }
    },
    mounted() {
        this.QueryBooks()
    }
}
</script>

<style scoped>
.form-item {
    margin: 12px 20px;
    font-weight: bold;
    font-size: 14px;
}
</style>