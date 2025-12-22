#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
#include<string>    //getline函数需要
#include<sstream>   //添加sstream用于字符串分隔
#include<cstdlib>   //rand()
#include<ctime>
#include<type_traits>
#include<chrono>
#include<cmath>     //isfinite()检查浮点数
#include<limits>    //用于数值范围常量
#include<fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
using namespace std;

//TODO:把debug改为release
//TODO:代码重复度高
vector<int> intNumbers;
vector<double> doubleNumbers;
vector<string> stringNumbers;

enum DataType { TYPE_INT, TYPE_DOUBLE, TYPE_STRING };
DataType currentDataType = TYPE_INT;    //默认一下
enum SortOrder { ORDER_ASC, ORDER_DESC };  // 升序，降序
SortOrder currentOrder = ORDER_ASC;

const bool AUTO_SAVE_ENABLED = true;
const string AUTOSAVE_FILENAME = "autosave.dat";

class SimpleSortException {
private:
    string message;
public:
    SimpleSortException(const string& msg) : message(" ⚠️ " + msg) {}
    string what() const { return message; }
};

sf::RenderWindow* visualWindow = nullptr;
const int WINDOW_WIDTH = 1200;  // 增加窗口宽度
const int WINDOW_HEIGHT = 700;  // 增加窗口高度
const int MIN_BAR_WIDTH = 2;    // 最小柱子宽度
const int MAX_BAR_WIDTH = 20;   // 最大柱子宽度

void mainMenu() {
    cout << "\n====== 排序系统 ======" << endl;
    cout << "当前数据类型: " << (currentDataType == TYPE_INT ? "整数" : (currentDataType == TYPE_DOUBLE ? "浮点数" : "字符串")) << endl;
    cout << "1.选择数据类型" << endl;
    cout << "2.输入数据（手动/随机）" << endl;
    cout << "3.选择排序算法并执行" << endl;
    cout << "4.显示当前数据" << endl;
    cout << "5.文件操作（保存/加载）" << endl;
    cout << "6.查看算法复杂度" << endl;
    cout << "0.退出系统" << endl;
    cout << "请选择: ";
}

void sortingMenu() {
    cout << "\n====== 选择排序算法 ======" << endl;
    cout << "注：只有整数类型的冒泡、插入、选择排序可以可视化……" << endl;
    cout << "1.冒泡排序（稳定）" << endl;
    cout << "2.插入排序（稳定）" << endl;
    cout << "3.选择排序（不稳定）" << endl;
    cout << "4.快速排序（不稳定）" << endl;
    cout << "5.归并排序（稳定）" << endl;
    cout << "0.返回上级菜单" << endl;
    cout << "请选择: ";
}

template<typename T>
void drawVisualization(const vector<T>& data, int highlight1 = -1, int highlight2 = -1, int step = 0) {
    if (!visualWindow) return;

    // 处理窗口事件
    sf::Event event;
    while (visualWindow->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            visualWindow->close();
            delete visualWindow;
            visualWindow = nullptr;
            return;
        }
    }

    // 清屏
    visualWindow->clear(sf::Color(40, 44, 52)); 

    if (data.empty()) return;

    // 找到最大值用于归一化
    double maxVal = 1.0;
    if constexpr (is_same_v<T, int>) {
        auto maxElement = max_element(data.begin(), data.end());
        if (maxElement != data.end()) {
            maxVal = static_cast<double>(*maxElement);
        }
    }
    else if constexpr (is_same_v<T, double>) {
        auto maxElement = max_element(data.begin(), data.end());
        if (maxElement != data.end()) {
            maxVal = *maxElement;
        }
    }
    else {
        // 对于字符串，使用长度
        maxVal = 100.0; // 固定值
    }

    if (maxVal <= 0) maxVal = 1.0;

    // 动态计算柱子宽度
    int barCount = data.size();
    float availableWidth = WINDOW_WIDTH - 20; // 留出边距
    float barWidth = availableWidth / barCount;

    // 限制柱子宽度范围
    if (barWidth < MIN_BAR_WIDTH) barWidth = MIN_BAR_WIDTH;
    if (barWidth > MAX_BAR_WIDTH) barWidth = MAX_BAR_WIDTH;

    // 计算总宽度和起始位置，使柱子居中
    float totalWidth = barCount * barWidth;
    float startX = (WINDOW_WIDTH - totalWidth) / 2;

    // 绘制所有柱子
    for (int i = 0; i < barCount; i++) {
        // 计算柱子的高度
        float value = 0.0f;
        if constexpr (is_same_v<T, int>) {
            value = static_cast<float>(data[i]);
        }
        else if constexpr (is_same_v<T, double>) {
            value = static_cast<float>(data[i]);
        }
        else {
            value = static_cast<float>(data[i].length()) * 10.0f; // 字符串按长度显示
        }

        float height = (value / maxVal) * (WINDOW_HEIGHT - 100);
        if (height < 2) height = 2;

        // 创建柱子
        sf::RectangleShape bar(sf::Vector2f(barWidth - 1, height));
        bar.setPosition(startX + i * barWidth, WINDOW_HEIGHT - height - 50);

        // 设置颜色：高亮或普通
        if (i == highlight1 || i == highlight2) {
            // 高亮颜色 - 红色表示比较或交换
            bar.setFillColor(sf::Color(220, 50, 47)); // 红色
        }
        else {
            // 根据数值设置渐变色
            float ratio = value / maxVal;
            sf::Uint8 r = static_cast<sf::Uint8>(100 + ratio * 155);
            sf::Uint8 g = static_cast<sf::Uint8>(150 + ratio * 105);
            sf::Uint8 b = static_cast<sf::Uint8>(200 + ratio * 55);
            bar.setFillColor(sf::Color(r, g, b));
        }

        visualWindow->draw(bar);
    }

    // 绘制基本信息（不使用字体，避免加载问题）
    // 创建一个简单的矩形作为标题背景
    sf::RectangleShape titleBg(sf::Vector2f(WINDOW_WIDTH, 30));
    titleBg.setPosition(0, 0);
    titleBg.setFillColor(sf::Color(60, 64, 72));
    visualWindow->draw(titleBg);

    // 绘制简单的图形表示信息
    // 数据大小指示器
    sf::RectangleShape sizeIndicator(sf::Vector2f(10, 10));
    sizeIndicator.setPosition(10, 10);
    sizeIndicator.setFillColor(sf::Color::White);
    visualWindow->draw(sizeIndicator);

    // 高亮说明
    if (highlight1 >= 0 || highlight2 >= 0) {
        sf::RectangleShape highlightIndicator(sf::Vector2f(10, 10));
        highlightIndicator.setPosition(WINDOW_WIDTH - 120, 10);
        highlightIndicator.setFillColor(sf::Color::Red);
        visualWindow->draw(highlightIndicator);
    }

    // 绘制步骤计数器（使用简单的图形）
    if (step > 0) {
        // 绘制步骤指示器
        for (int i = 0; i < min(step / 10, 20); i++) {
            sf::RectangleShape stepDot(sf::Vector2f(3, 3));
            stepDot.setPosition(150 + i * 5, 15);
            stepDot.setFillColor(sf::Color::Green);
            visualWindow->draw(stepDot);
        }
    }

    visualWindow->display();

    // 根据数据量调整延迟时间
    int delay = 50; // 默认延迟
    if (data.size() > 100) delay = 20;
    if (data.size() > 500) delay = 5;
    if (data.size() > 1000) delay = 1;

    sf::sleep(sf::milliseconds(delay));
}

// 初始化可视化窗口
void initVisualizationWindow() {
    if (visualWindow) {
        delete visualWindow;
    }
    visualWindow = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Sorting Visualization");
}

// 关闭可视化窗口
void closeVisualizationWindow() {
    if (visualWindow) {
        visualWindow->close();
        delete visualWindow;
        visualWindow = nullptr;
    }
}

void selectDataType() {
    cout << "\n====== 选择数据类型 ======" << endl;
    cout << "1.整数（int）" << endl;
    cout << "2.浮点数（double）" << endl;
    cout << "3.字符串（string）" << endl;
    cout << "请选择: ";
    int choice = 1;
    cin >> choice;
    cin.ignore();
    if (choice == 1) {
        currentDataType = TYPE_INT;
        cout << "已切换到整数类型" << endl;
    }
    else if (choice == 2) {
        currentDataType = TYPE_DOUBLE;
        cout << "已切换到浮点数类型" << endl;
    }
    else if (choice == 3) {
        currentDataType = TYPE_STRING;
        cout << "已切换到字符串类型" << endl;
    }
    else {
        cout << "无效选择，保持当前类型" << endl;
    }
}

bool validateInt(int value) {
    const int MIN_INT = (numeric_limits<int>::min)();  // -2147483648
    const int MAX_INT = (numeric_limits<int>::max)();  // 2147483647
    if (value < MIN_INT || value > MAX_INT) {
        cout << "警告：整数 " << value << " 可能超出处理范围" << endl;
        return false;
    }
    return true;
}

bool validateDouble(double value) {
    if (!isfinite(value)) {
        cout << "错误：浮点数 " << value << " 不是有效数值" << endl;
        return false;
    }
    return true;
}

template<typename T>
bool validateValue(const T& value) {
    if constexpr (is_same_v<T, int>) {
        return validateInt(value);
    }
    else if constexpr (is_same_v<T, double>) {
        return validateDouble(value);
    }
    else {
        // 未知类型，默认通过
        return true;
    }
}

template<typename T>
vector<T> inputNumbers() {
    string typeName;
    if constexpr (is_same_v<T, int>) {
        typeName = "整数";
    }
    else if constexpr (is_same_v<T, double>) {
        typeName = "浮点数";
    }
    else if constexpr (is_same_v<T, string>) {
        typeName = "字符串";
    }
    cout << "请输入" << typeName << "（空格分隔，回车结束）: ";

    vector<T> nums;  // 创建一个空数组
    string line;
    getline(cin, line); //读取一整行，包含回车
    if (line.empty()) {
        cout << "警告：没有输入任何内容！" << endl;
        return nums;
    }
    stringstream ss(line);//使用字符串流解析数字
    T num{};
    int totalCount = 0;
    int validCount = 0;

    if constexpr (is_same_v<T, int>) {
        string token;
        while (ss >> token) {
            totalCount++;
            try {
                // 使用stoi，它会抛出异常如果转换失败
                int num = stoi(token);
                if (validateValue(num)) {
                    nums.push_back(num);
                    validCount++;
                }
                else {
                    cout << "  位置" << totalCount << "的数据 '" << token << "' 被跳过" << endl;
                }
            }
            catch (const invalid_argument& e) {
                cout << "  位置" << totalCount << "的数据 '" << token << "' 不是有效整数" << endl;
            }
            catch (const out_of_range& e) {
                cout << "  位置" << totalCount << "的数据 '" << token << "' 超出整数范围" << endl;
            }
        }
    }
    else if constexpr (is_same_v<T, double>) {
        string token;
        while (ss >> token) {
            totalCount++;

            try {
                // 使用stod转换，它会处理inf和nan
                double num = stod(token);

                // 验证转换后的值
                if (validateValue(num)) {
                    nums.push_back(num);
                    validCount++;
                }
                else {
                    cout << "  位置" << totalCount << "的数据 '" << token << "' 被跳过" << endl;
                }
            }
            catch (const invalid_argument& e) {
                cout << "  位置" << totalCount << "的数据 '" << token << "' 不是有效浮点数" << endl;
            }
            catch (const out_of_range& e) {
                cout << "  位置" << totalCount << "的数据 '" << token << "' 超出浮点数范围" << endl;
            }
        }
    }
    else {
        T num{};
        while (ss >> num) {
            totalCount++;
            if (validateValue(num)) {
                nums.push_back(num);
                validCount++;
            }
            else {
                cout << "  位置" << totalCount << "的数据被跳过" << endl;
            }
        }
    }

    if (nums.empty()) {
        cout << "警告：没有输入任何有效数据！" << endl;
    }
    else {
        cout << "成功输入" << validCount << "个有效数据";
        if (validCount < totalCount) {
            cout << "（跳过 " << (totalCount - validCount) << " 个无效数据）";
        }
        cout << endl;
    }
    return nums;
}

template<typename T>
void checkMemorySafe(const vector<T>& data) {
    size_t elementSize = sizeof(T);
    size_t totalSize = data.size() * elementSize;
    // 如果数据超过100MB，发出警告
    const size_t WARNING_SIZE = 100 * 1024 * 1024;  // 100MB

    if (totalSize > WARNING_SIZE) {
        cout << "警告：数据量较大 ("
            << totalSize / (1024 * 1024) << " MB)" << endl;
        cout << "排序可能需要较多内存和时间，是否继续？(y/n): ";
        string answer;
        getline(cin, answer);
        if (answer != "y" && answer != "Y") {
            throw SimpleSortException("用户取消了大数据排序");
        }
    }
}

template<typename T>
vector<T> generateRandomNumbers() {
    vector<T> randomData;
    cout << "请输入要生成的随机数据数量 (1-10000): ";
    string input;
    getline(cin, input);

    if (input.empty()) {
        cout << "没有输入数量，默认生成10个随机数" << endl;
        input = "10";
    }

    int count = 10;  // 默认值
    try {
        count = stoi(input);
        if (count <= 0) {
            cout << "数量必须大于0，使用默认值10" << endl;
            count = 10;
        }
        if (count > 10000) {
            cout << "数量太大，限制为10000" << endl;
            count = 10000;
        }
    }
    catch (...) {
        cout << "输入无效，使用默认值10" << endl;
        count = 10;
    }

    //先reverse预分配内存，一次性分配足够内存，避免多次扩容
    randomData.reserve(count);

    // 根据数据类型生成随机数
    if constexpr (is_same_v<T, int>) {
        cout << "正在生成 " << count << " 个随机整数 (0-999)..." << endl;
        for (int i = 0; i < count; i++) {
            randomData.push_back(rand() % 1000);  
        }
        cout << "已生成 " << count << " 个随机整数" << endl;
    }
    else if constexpr (is_same_v<T, double>) {
        cout << "正在生成 " << count << " 个随机浮点数 (0.00-99.99)..." << endl;
        for (int i = 0; i < count; i++) {
            // 生成两位小数的浮点数
            double value = (rand() % 10000) / 100.0;
            randomData.push_back(value);  
        }
        cout << "已生成 " << count << " 个随机浮点数" << endl;
    }
    else if constexpr (is_same_v<T, string>) {
        const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        cout << "正在生成 " << count << " 个随机字符串 (3-6个字母)..." << endl;

        for (int i = 0; i < count; i++) {
            int length = 3 + rand() % 4;  // 3-6个字符
            string randomStr;
            randomStr.reserve(length); 
            for (int j = 0; j < length; j++) {
                randomStr += chars[rand() % chars.size()];
            }
            randomData.push_back(randomStr);  
        }
        cout << "已生成 " << count << " 个随机字符串" << endl;
    }

    return randomData; 
}

template<typename T>
void outputNumbers(const vector<T>& nums) {// const &：不修改数值
    if (nums.empty()) {
        cout << "数据为空" << endl;
        return;
    }
    cout << "数据（" << nums.size() << "个）：";
    for (int i = 0; i < nums.size(); i++) {
        cout << nums[i];
        if (i < nums.size() - 1) cout << " ";
    }
    cout << endl;
}

template<typename T>
void bubbleSortVisual(vector<T>& numbers, SortOrder order = ORDER_ASC) {
    int n = numbers.size();
    int step = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            step++;
            // 每10步绘制一次，减少绘制频率
            if (step % 10 == 0) {
                drawVisualization(numbers, j, j + 1, step);
            }

            bool shouldSwap = false;
            if (order == ORDER_ASC) {
                shouldSwap = numbers[j] > numbers[j + 1];
            }
            else {
                shouldSwap = numbers[j] < numbers[j + 1];
            }

            if (shouldSwap) {
                swap(numbers[j], numbers[j + 1]);
                // 交换后绘制一次
                if (step % 5 == 0) {
                    drawVisualization(numbers, j, j + 1, step);
                }
            }
        }
        // 每轮结束后绘制一次
        drawVisualization(numbers, -1, -1, step);
    }
}

template<typename T>
void insertionSortVisual(vector<T>& numbers, SortOrder order = ORDER_ASC) {
    int n = numbers.size();
    int step = 0;
    for (int i = 1; i < n; i++) {
        T key = numbers[i];
        int j = i - 1;
        step++;

        if (step % 5 == 0) {
            drawVisualization(numbers, i, j, step);
        }

        if (order == ORDER_ASC) {
            while (j >= 0 && numbers[j] > key) {
                numbers[j + 1] = numbers[j];
                j--;
                step++;
                if (step % 10 == 0) {
                    drawVisualization(numbers, j + 1, j, step);
                }
            }
        }
        else {
            while (j >= 0 && numbers[j] < key) {
                numbers[j + 1] = numbers[j];
                j--;
                step++;
                if (step % 10 == 0) {
                    drawVisualization(numbers, j + 1, j, step);
                }
            }
        }
        numbers[j + 1] = key;
        if (step % 20 == 0) {
            drawVisualization(numbers, j + 1, -1, step);
        }
    }
}

template<typename T>
void selectionSortVisual(vector<T>& numbers, SortOrder order = ORDER_ASC) {
    int n = numbers.size();
    int step = 0;
    for (int i = 0; i < n - 1; i++) {
        int selectedIndex = i;

        step++;
        if (step % 10 == 0) {
            drawVisualization(numbers, i, selectedIndex, step);
        }

        for (int j = i + 1; j < n; j++) {
            step++;
            if (step % 20 == 0) {
                drawVisualization(numbers, selectedIndex, j, step);
            }

            if (order == ORDER_ASC) {
                if (numbers[j] < numbers[selectedIndex]) {
                    selectedIndex = j;
                }
            }
            else {
                if (numbers[j] > numbers[selectedIndex]) {
                    selectedIndex = j;
                }
            }
        }
        if (selectedIndex != i) {
            swap(numbers[i], numbers[selectedIndex]);
            step++;
            if (step % 10 == 0) {
                drawVisualization(numbers, i, selectedIndex, step);
            }
        }
        // 每轮结束后绘制一次
        if (i % 5 == 0) {
            drawVisualization(numbers, -1, -1, step);
        }
    }
}

template<typename T>
void bubbleSort(vector<T>& numbers, SortOrder order = ORDER_ASC) {
    int n = numbers.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (order == ORDER_ASC) {
                if (numbers[j] > numbers[j + 1]) {
                    swap(numbers[j], numbers[j + 1]);
                }
            }
            else {
                if (numbers[j] < numbers[j + 1]) {
                    swap(numbers[j], numbers[j + 1]);
                }
            }
        }
    }
}

template<typename T>
void insertionSort(vector<T>& numbers, SortOrder order = ORDER_ASC) {
    int n = numbers.size();
    for (int i = 1; i < n; i++) {
        T key = numbers[i];   //i从第二个数开始，key是当前的数值
        int j = i - 1;
        if (order == ORDER_ASC) {
            while (j >= 0 && numbers[j] > key) {
                numbers[j + 1] = numbers[j];
                j--;
            }
        }
        else {
            while (j >= 0 && numbers[j] < key) {
                numbers[j + 1] = numbers[j];
                j--;
            }
        }
        numbers[j + 1] = key;
    }
}

template<typename T>
void selectionSort(vector<T>& numbers, SortOrder order = ORDER_ASC) {
    int n = numbers.size();
    for (int i = 0; i < n - 1; i++) {
        int selectedIndex = i;
        for (int j = i + 1; j < n; j++) {
            if (order == ORDER_ASC) {
                if (numbers[j] < numbers[selectedIndex]) {
                    selectedIndex = j;
                }
            }
            else {
                if (numbers[j] > numbers[selectedIndex]) {
                    selectedIndex = j;
                }
            }
        }
        if (selectedIndex != i) {
            swap(numbers[i], numbers[selectedIndex]);
        }
    }
}

template<typename T>
int partition(vector<T>& numbers, int low, int high, SortOrder order) {
    // 随机选择基准值
    int randomIndex = low + rand() % (high - low + 1);
    swap(numbers[low], numbers[randomIndex]);

    T pivot = numbers[low];

    while (low < high) {
        if (order == ORDER_ASC) {
            // 升序：把大于等于pivot的放右边
            while (low < high && numbers[high] >= pivot) --high;
        }
        else {
            // 降序：把小于等于pivot的放右边
            while (low < high && numbers[high] <= pivot) --high;  // 改成 <=
        }
        numbers[low] = numbers[high];

        if (order == ORDER_ASC) {
            // 升序：把小于等于pivot的放左边
            while (low < high && numbers[low] <= pivot) ++low;
        }
        else {
            // 降序：把大于等于pivot的放左边
            while (low < high && numbers[low] >= pivot) ++low;    // 改成 >=
        }
        numbers[high] = numbers[low];
    }

    numbers[low] = pivot;
    return low;
}
template<typename T>
void quickSort(vector<T>& numbers, SortOrder order = ORDER_ASC, int low = 0, int high = -1) {
    if (high == -1) high = numbers.size() - 1;

    while (low < high && !numbers.empty()) {
        int pivotPos = partition(numbers, low, high, order);
        // 总是先递归处理较短的部分
        if (pivotPos - low < high - pivotPos) {
            quickSort(numbers, order, low, pivotPos - 1);
            low = pivotPos + 1;
        }
        else {
            quickSort(numbers, order, pivotPos + 1, high);
            high = pivotPos - 1;
        }
    }
}

template<typename T>
void merge(vector<T>& arr, int left, int mid, int right, SortOrder order) {
    // 创建临时数组
    vector<T> temp(right - left + 1);
    int i = left;      // 左半部分起始索引
    int j = mid + 1;   // 右半部分起始索引
    int k = 0;         // 临时数组索引

    // 合并两个有序数组
    while (i <= mid && j <= right) {
        if (order == ORDER_ASC) {
            if (arr[i] <= arr[j]) {
                temp[k++] = arr[i++];
            }
            else {
                temp[k++] = arr[j++];
            }
        }
        else {
            if (arr[i] >= arr[j]) {
                temp[k++] = arr[i++];
            }
            else {
                temp[k++] = arr[j++];
            }
        }
    }

    // 拷贝剩余元素
    while (i <= mid) {
        temp[k++] = arr[i++];
    }
    while (j <= right) {
        temp[k++] = arr[j++];
    }

    // 拷贝回原数组
    for (int p = 0; p < k; p++) {
        arr[left + p] = temp[p];
    }
}
template<typename T>
void mergeSort(vector<T>& arr, SortOrder order = ORDER_ASC, int left = 0, int right = -1) {
    // 设置默认的right值
    if (right == -1) {
        right = arr.size() - 1;
    }

    // 递归终止条件
    if (left >= right) {
        return;
    }

    // 计算中间位置
    int mid = left + (right - left) / 2;

    // 递归排序左半部分和右半部分
    mergeSort(arr, order, left, mid);
    mergeSort(arr, order, mid + 1, right);
    // 合并两个有序部分
    merge(arr, left, mid, right, order);
}

template<typename T>
void saveToFile(const vector<T>& data, const string& filename) {
    try {
        ifstream testFile(filename);
        if (testFile.is_open()) {
            testFile.close();
            cout << "文件 " << filename << " 已存在，覆盖吗？(y/n): ";
            string answer;
            getline(cin, answer);
            if (answer != "y" && answer != "Y") {
                cout << "取消保存" << endl;
                return;
            }
            cout << "将覆盖原文件" << endl;
        }
        ofstream file(filename);
        if (!file.is_open()) {
            throw SimpleSortException("无法创建文件: " + filename);
        }
        if constexpr (is_same_v<T, int>) {
            file << "INT" << endl;
        }
        else if constexpr (is_same_v<T, double>) {
            file << "DOUBLE" << endl;
        }
        else if constexpr (is_same_v<T, string>) {
            file << "STRING" << endl;
        }

        file << data.size() << endl;

        for (const auto& item : data) {
            file << item << " ";
        }
        file << endl;

        cout << "数据已保存到 " << filename
            << "（" << data.size() << " 个元素）" << endl;
    }
    catch (const exception& e) {
        throw SimpleSortException(string("保存文件失败: ") + e.what());
    }
}

template<typename T>
vector<T> loadFromFile(const string& filename) {
    vector<T> data;
    try {
        ifstream file(filename);
        if (!file.is_open()) {
            throw SimpleSortException("找不到文件: " + filename);
        }

        string fileType;
        file >> fileType;

        string expectedType;
        if constexpr (is_same_v<T, int>) {
            expectedType = "INT";
        }
        else if constexpr (is_same_v<T, double>) {
            expectedType = "DOUBLE";
        }
        else if constexpr (is_same_v<T, string>) {
            expectedType = "STRING";
        }

        if (fileType != expectedType) {
            throw SimpleSortException("文件类型不匹配！文件里是: " + fileType + "，但需要: " + expectedType);
        }

        size_t count;
        file >> count;
        if (count > 1000000) {  // 限制最大加载数量
            throw SimpleSortException("文件数据量过大 (" + to_string(count) + " 个)，可能内存不足");
        }

        data.resize(count);
        for (size_t i = 0; i < count; i++) {
            if (!(file >> data[i])) {
                throw SimpleSortException("读取文件数据时出错");
            }
        }

        cout << "从 " << filename << " 加载了 " << count << " 个数据" << endl;

    }
    catch (const SimpleSortException&) {
        throw;  // 重新抛出自定义异常
    }
    catch (const exception& e) {
        throw SimpleSortException(string("加载文件失败: ") + e.what());
    }

    return data;
}

template<typename T>
void askToSaveSortedData(const vector<T>& data, DataType type) {
    cout << "\n===== 是否保存排序结果？ =====" << endl;
    cout << "1. 保存到文件" << endl;
    cout << "2. 不保存，继续操作" << endl;
    cout << "请选择 (直接回车不保存): ";

    string choiceInput;
    getline(cin, choiceInput);
    if (choiceInput.empty()) {
        cout << "排序结果未保存" << endl;
        return;
    }
    int choice = 0;
    stringstream ss(choiceInput);
    if (!(ss >> choice) || choice != 1) {
        cout << "排序结果未保存" << endl;
        return;
    }

    cout << "请输入文件名（直接回车使用默认名）: ";
    string filename;
    getline(cin, filename);
    if (filename.empty()) {
        if (type == TYPE_INT) {
            filename = "sorted_int.txt";
        }
        else if (type == TYPE_DOUBLE) {
            filename = "sorted_double.txt";
        }
        else {
            filename = "sorted_string.txt";
        }
        cout << "使用默认文件名: " << filename << endl;
    }
    saveToFile(data, filename);
}

void showAlgorithmComplexity() {
    cout << "\n====== 排序算法复杂度 ======" << endl;
    cout << "1.冒泡排序：" << endl;
    cout << "     时间复杂度：平均O(n²)，最坏O(n²)，最好O(n)" << endl;
    cout << "     空间复杂度：O(1)" << endl;
    cout << "     稳定性：稳定" << endl;

    cout << "2.插入排序：" << endl;
    cout << "     时间复杂度：平均O(n²)，最坏O(n²)，最好O(n)" << endl;
    cout << "     空间复杂度：O(1)" << endl;
    cout << "     稳定性：稳定" << endl;

    cout << "3.选择排序：" << endl;
    cout << "     时间复杂度：O(n²)" << endl;
    cout << "     空间复杂度：O(1)" << endl;
    cout << "     稳定性：不稳定" << endl;

    cout << "4.快速排序：" << endl;
    cout << "     时间复杂度：平均O(nlog2 n)，最坏O(n²)，最好O(nlog2 n)" << endl;
    cout << "     空间复杂度：O(log2 n)" << endl;
    cout << "     稳定性：不稳定" << endl;

    cout << "5.归并排序：" << endl;
    cout << "     时间复杂度：O(nlog2 n)" << endl;
    cout << "     空间复杂度：O(n)" << endl;
    cout << "     稳定性：稳定" << endl;
}


// 自动保存当前状态（在每次操作后调用）
void autoSave() {
    const string tmpFile = AUTOSAVE_FILENAME + ".tmp";
    const string finalFile = AUTOSAVE_FILENAME;
    remove(tmpFile.c_str());

    // 创建独立作用域，确保 ofstream 被强制关闭
    {
        ofstream ofs(tmpFile, ios::binary); 
        if (!ofs) {
            throw SimpleSortException("无法创建临时文件: " + tmpFile);
        }

        if (currentDataType == TYPE_INT) {
            ofs << "INT\n" << (currentOrder == ORDER_ASC ? "ASC" : "DESC") << "\n";
            ofs << intNumbers.size() << "\n";
            for (const auto& x : intNumbers) ofs << x << " ";
        }
        else if (currentDataType == TYPE_DOUBLE) {
            ofs << "DOUBLE\n" << (currentOrder == ORDER_ASC ? "ASC" : "DESC") << "\n";
            ofs << doubleNumbers.size() << "\n";
            for (const auto& x : doubleNumbers) ofs << x << " ";
        }
        else {
            ofs << "STRING\n" << (currentOrder == ORDER_ASC ? "ASC" : "DESC") << "\n";
            ofs << stringNumbers.size() << "\n";
            for (const auto& x : stringNumbers) {
                string escaped = x;
                for (char& c : escaped) {
                    if (c == ' ') c = '\x01';
                    if (c == '\n') c = '\x02';
                }
                ofs << escaped << "\n"; 
            }
        }
        ofs << "\n";  

        // 强制刷盘并检查错误
        ofs.flush();
        if (!ofs.good()) {
            throw SimpleSortException("写入数据失败");
        }
    }  

    // 在文件完全关闭后，执行原子重命名，先删除目标文件
    remove(finalFile.c_str());
    int result = rename(tmpFile.c_str(), finalFile.c_str());
    // 检查重命名结果
    if (result != 0) {
        int err = errno;
        string errorMsg = "无法完成保存，错误 " + to_string(err) + ": " + strerror(err);
        // Windows 常见错误 13 = 权限拒绝（文件被占用）
        throw SimpleSortException(errorMsg);
    }
}

// 程序启动时加载自动保存的数据
bool autoLoad() {
    ifstream ifs(AUTOSAVE_FILENAME.c_str());
    if (!ifs.is_open()) return false;

    try {
        string typeStr, orderStr;
        size_t count;

        // 读取类型
        if (!getline(ifs, typeStr)) return false;
        if (typeStr == "INT") currentDataType = TYPE_INT;
        else if (typeStr == "DOUBLE") currentDataType = TYPE_DOUBLE;
        else if (typeStr == "STRING") currentDataType = TYPE_STRING;
        else return false;

        // 读取顺序
        if (!getline(ifs, orderStr)) return false;
        currentOrder = (orderStr == "ASC") ? ORDER_ASC : ORDER_DESC;

        // 读取个数
        if (!(ifs >> count)) return false;
        ifs.ignore(numeric_limits<streamsize>::max(), '\n'); // 跳过换行

        // 读取数据
        if (currentDataType == TYPE_INT) {
            intNumbers.clear();
            intNumbers.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                int x; ifs >> x;
                intNumbers.push_back(x);
            }
        }
        else if (currentDataType == TYPE_DOUBLE) {
            doubleNumbers.clear();
            doubleNumbers.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                double x; ifs >> x;
                doubleNumbers.push_back(x);
            }
        }
        else {
            stringNumbers.clear();
            stringNumbers.reserve(count);
            ifs.ignore(); 
            for (size_t i = 0; i < count; ++i) {
                string x;
                getline(ifs, x);
                for (char& c : x) {
                    if (c == '\x01') c = ' ';
                    if (c == '\x02') c = '\n';
                }
                if (!x.empty()) stringNumbers.push_back(move(x));
            }
        }

        cout << "已自动恢复上次的数据：" << " 类型: " << typeStr << "，数量: " << count << endl;
        return true;
    }
    catch (const exception& e) {
        cerr << "自动加载失败，启动空数据: " << e.what() << endl;
        // 清空数据防止半加载状态
        intNumbers.clear(); doubleNumbers.clear(); stringNumbers.clear();
        return false;
    }
}

int main() {
    srand(time(nullptr));
    cout << "欢迎使用排序系统（支持整数/浮点数/字符串）" << endl;
   
    ifstream test(AUTOSAVE_FILENAME);
    if (test.is_open()) {
        test.seekg(0, ios::end);
        size_t size = test.tellg();
        cout << "发现 " << AUTOSAVE_FILENAME
            << "，大小 " << size << " 字节" << endl;
        if (size == 0) {
            cout << "报告：自动保存文件为空！" << endl;
        }
        test.close();
    }
    else {
        cout << "报告：自动保存文件不存在" << endl;
    }

    //立即加载数据
    if (autoLoad()) {
        cout << "已自动加载上次的数据！" << endl;
        // 显示加载了什么数据
        if (currentDataType == TYPE_INT) {
            cout << "当前有 " << intNumbers.size() << " 个整数" << endl;
        }
        else if (currentDataType == TYPE_DOUBLE) {
            cout << "当前有 " << doubleNumbers.size() << " 个浮点数" << endl;
        }
        else {
            cout << "当前有 " << stringNumbers.size() << " 个字符串" << endl;
        }
    }
    else {
        cout << "未找到可加载的数据，请手动输入" << endl;
    }

    int mainChoice = 0;
    do {
        try {
            mainMenu();
            string input;
            getline(cin, input);
            if (input.empty()) {
                cout << "请输入数字！" << endl;
                continue;
            }
            stringstream ss(input);
            if (!(ss >> mainChoice)) {
                cout << "无效输入，请输入数字！" << endl;
                continue;
            }
            string extra;
            if (ss >> extra) {
                cout << "注意：检测到额外输入 '" << extra << "'，请重新输入单数字选项！" << endl;
                continue;  // 跳过本次循环，让用户重新输入
            }

            switch (mainChoice) {
            case 1: selectDataType(); autoSave(); break;
            case 2:  // 输入数据
            {
                cout << "\n--- 输入数据 ---" << endl;
                cout << "当前数据类型: " << (currentDataType == TYPE_INT ? "整数" :
                    (currentDataType == TYPE_DOUBLE ? "浮点数" : "字符串")) << endl;

                cout << "\n选择输入方式:" << endl;
                cout << "1. 手动输入（空格分隔）" << endl;
                cout << "2. 生成随机数据" << endl;
                cout << "请选择: ";

                string choiceInput;
                getline(cin, choiceInput);

                int choice = 1;  // 默认手动输入

                if (!choiceInput.empty()) {
                    try {
                        choice = stoi(choiceInput);
                    }
                    catch (...) {
                        cout << "输入无效，使用手动输入" << endl;
                        choice = 1;
                    }
                }

                if (choice == 1) {
                    // 手动输入
                    if (currentDataType == TYPE_INT) {
                        intNumbers = inputNumbers<int>();
                    }
                    else if (currentDataType == TYPE_DOUBLE) {
                        doubleNumbers = inputNumbers<double>();
                    }
                    else {
                        stringNumbers = inputNumbers<string>();
                    }
                }
                else if (choice == 2) {
                    // 生成随机数据
                    cout << "\n--- 生成随机数据 ---" << endl;
                    if (currentDataType == TYPE_INT) {
                        intNumbers = generateRandomNumbers<int>();
                        cout << "生成的随机整数: ";
                        outputNumbers(intNumbers);
                    }
                    else if (currentDataType == TYPE_DOUBLE) {
                        doubleNumbers = generateRandomNumbers<double>();
                        cout << "生成的随机浮点数: ";
                        outputNumbers(doubleNumbers);
                    }
                    else {
                        stringNumbers = generateRandomNumbers<string>();
                        cout << "生成的随机字符串: ";
                        outputNumbers(stringNumbers);
                    }
                }
                else {
                    cout << "无效选择，使用手动输入" << endl;
                    if (currentDataType == TYPE_INT) {
                        intNumbers = inputNumbers<int>();
                    }
                    else if (currentDataType == TYPE_DOUBLE) {
                        doubleNumbers = inputNumbers<double>();
                    }
                    else {
                        stringNumbers = inputNumbers<string>();
                    }
                }
                autoSave();
                break;
            }

            case 3:
            {
                currentOrder = ORDER_ASC; //复位
                if ((currentDataType == TYPE_INT && intNumbers.empty()) ||
                    (currentDataType == TYPE_DOUBLE && doubleNumbers.empty()) ||
                    (currentDataType == TYPE_STRING && stringNumbers.empty())) {
                    cout << "错误！请先输入数据！" << endl;
                    break;
                }
                int sortChoice = 0;
                sortingMenu();
                string sortInput;
                getline(cin, sortInput);
                if (sortInput.empty()) {
                    cout << "请输入数字选择排序算法！" << endl;
                    break;
                }
                stringstream sortSS(sortInput);
                if (!(sortSS >> sortChoice)) {
                    cout << "无效的排序算法选择！" << endl;
                    break;
                }
                string sortExtra;
                if (sortSS >> sortExtra) {
                    cout << "注意：检测到额外输入 '" << sortExtra << "'，已忽略" << endl;
                }
                if (sortChoice == 0) {
                    cout << "返回上级菜单" << endl;
                    break;
                }
                if (sortChoice < 1 || sortChoice > 5) {
                    cout << "无效选择，请输入1-5之间的数字！" << endl;
                    break;
                }

                cout << "\n====== 选择排序方向 ======" << endl;
                cout << "1. 升序（从小到大）" << endl;
                cout << "2. 降序（从大到小）" << endl;
                cout << "请选择（直接回车使用默认升序）: ";
                string orderInput;
                getline(cin, orderInput);
                if (!orderInput.empty()) {
                    int orderChoice = 1;
                    stringstream orderSS(orderInput);
                    if (orderSS >> orderChoice) {
                        if (orderChoice == 2) {
                            currentOrder = ORDER_DESC;
                            cout << "将按降序排序" << endl;
                        }
                        else {
                            cout << "将按升序排序" << endl;
                        }
                    }
                    else {
                        cout << "输入无效，使用默认升序" << endl;
                    }
                }
                else {
                    cout << "使用默认升序" << endl;
                }

                bool useVisualization = false;
                if (currentDataType == TYPE_INT && sortChoice >= 1 && sortChoice <= 3) {
                    cout << "\n是否为当前排序启用可视化？(y/n，或直接回车跳过): ";
                    string visInput;
                    getline(cin, visInput);
                    if (!visInput.empty() && (visInput == "y" || visInput == "Y")) {
                        useVisualization = true;
                        cout << "已启用可视化...准备显示排序过程" << endl;
                    }
                }

                cout << "\n排序前：";
                try {
                    if (currentDataType == TYPE_INT) {
                        outputNumbers(intNumbers);
                        checkMemorySafe(intNumbers);

                        long long sortTime = 0;
                        auto startTime = chrono::high_resolution_clock::now();

                        if (useVisualization) {
                            initVisualizationWindow();
                            // 显示初始状态
                            drawVisualization(intNumbers);

                            cout << "正在使用";
                            switch (sortChoice) {
                            case 1: cout << "冒泡排序（稳定）可视化"; bubbleSortVisual(intNumbers, currentOrder); break;
                            case 2: cout << "插入排序（稳定）可视化"; insertionSortVisual(intNumbers, currentOrder); break;
                            case 3: cout << "选择排序（不稳定）可视化"; selectionSortVisual(intNumbers, currentOrder); break;
                            case 4: cout << "快速排序（不稳定）"; quickSort(intNumbers, currentOrder); break;
                            case 5: cout << "归并排序（稳定）"; mergeSort(intNumbers, currentOrder); break;
                            default: cout << "无效选择"; continue;
                            }
                            drawVisualization(intNumbers);  // 显示最终状态
                            sf::sleep(sf::seconds(2));      // 保持窗口2秒
                            closeVisualizationWindow();
                        }
                        else {
                            cout << "正在使用";
                            switch (sortChoice) {
                            case 1: cout << "冒泡排序（稳定）"; bubbleSort(intNumbers, currentOrder); break;
                            case 2: cout << "插入排序（稳定）"; insertionSort(intNumbers, currentOrder); break;
                            case 3: cout << "选择排序（不稳定）"; selectionSort(intNumbers, currentOrder); break;
                            case 4: cout << "快速排序（不稳定）"; quickSort(intNumbers, currentOrder); break;
                            case 5: cout << "归并排序（稳定）"; mergeSort(intNumbers, currentOrder); break;
                            default: cout << "无效选择"; continue;
                            }
                            cout << "进行排序……" << endl;
                        }

                        auto endTime = chrono::high_resolution_clock::now();
                        sortTime = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();

                        cout << "排序后：";
                        outputNumbers(intNumbers);
                        if (!useVisualization) {  // 只有非可视化时才显示耗时
                            cout << "性能统计：排序耗时 " << sortTime << " 微秒" << endl;
                        }
                        autoSave();
                        askToSaveSortedData(intNumbers, TYPE_INT);
                        break;
                    }
                    else if (currentDataType == TYPE_DOUBLE) {
                        outputNumbers(doubleNumbers);
                        checkMemorySafe(doubleNumbers);
                        cout << "正在使用";

                        long long sortTime = 0;
                        auto startTime = chrono::high_resolution_clock::now();
                        switch (sortChoice) {
                        case 1: cout << "冒泡排序（稳定）"; bubbleSort(doubleNumbers, currentOrder); break;
                        case 2: cout << "插入排序（稳定）"; insertionSort(doubleNumbers, currentOrder); break;
                        case 3: cout << "选择排序（不稳定）"; selectionSort(doubleNumbers, currentOrder); break;
                        case 4: cout << "快速排序（不稳定）"; quickSort(doubleNumbers, currentOrder); break;
                        case 5: cout << "归并排序（稳定）"; mergeSort(doubleNumbers, currentOrder); break;
                        default: cout << "无效选择"; continue;
                        }
                        auto endTime = chrono::high_resolution_clock::now();
                        sortTime = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();

                        cout << " 进行排序……" << endl;
                        cout << "排序后: ";
                        outputNumbers(doubleNumbers);
                        cout << "性能统计：排序耗时 " << sortTime << " 微秒" << endl;
                        autoSave();
                        askToSaveSortedData(doubleNumbers, TYPE_DOUBLE);
                        break;
                    }
                    else {
                        outputNumbers(stringNumbers);
                        checkMemorySafe(stringNumbers);
                        cout << "正在使用";

                        long long sortTime = 0;
                        auto startTime = chrono::high_resolution_clock::now();
                        switch (sortChoice) {
                        case 1: cout << "冒泡排序（稳定）"; bubbleSort(stringNumbers, currentOrder); break;
                        case 2: cout << "插入排序（稳定）"; insertionSort(stringNumbers, currentOrder); break;
                        case 3: cout << "选择排序（不稳定）"; selectionSort(stringNumbers, currentOrder); break;
                        case 4: cout << "快速排序（不稳定）"; quickSort(stringNumbers, currentOrder); break;
                        case 5: cout << "归并排序（稳定）"; mergeSort(stringNumbers, currentOrder); break;
                        default: cout << "无效选择"; continue;
                        }
                        auto endTime = chrono::high_resolution_clock::now();
                        sortTime = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();

                        cout << "进行排序……" << endl;
                        cout << "排序后：";
                        outputNumbers(stringNumbers);
                        cout << "性能统计：排序耗时 " << sortTime << " 微秒" << endl;
                        autoSave();
                        askToSaveSortedData(stringNumbers, TYPE_STRING);
                        break;
                    }
                }
                catch (const SimpleSortException& e) {
                    cout << e.what() << endl;
                }
                catch (const bad_alloc&) {
                    cout << "内存不足！无法完成排序" << endl;
                    cout << "建议：减少数据量或使用更省内存的算法" << endl;
                }
                catch (const exception& e) {
                    cout << "排序错误: " << e.what() << endl;
                }
                catch (...) {
                    cout << "排序过程中发生未知错误" << endl;
                }

            } break;

            case 4:
                cout << "当前数据：";
                if (currentDataType == TYPE_INT) {
                    outputNumbers(intNumbers);
                }
                else if (currentDataType == TYPE_DOUBLE) {
                    outputNumbers(doubleNumbers);
                }
                else {
                    outputNumbers(stringNumbers);
                }
                break;

            case 5:
            {
                int fileChoice;
                do {
                    cout << "\n====== 文件操作菜单 ======" << endl;
                    cout << "1. 保存当前数据到文件" << endl;
                    cout << "2. 从文件加载数据" << endl;
                    cout << "3. 保存排序结果" << endl;
                    cout << "0. 返回主菜单" << endl;
                    cout << "请选择: ";

                    string fileInput;
                    getline(cin, fileInput);
                    if (fileInput.empty()) {
                        cout << "请输入数字！" << endl;
                        continue;
                    }
                    stringstream fileSS(fileInput);
                    if (!(fileSS >> fileChoice)) {
                        cout << "无效输入！" << endl;
                        continue;
                    }

                    switch (fileChoice) {
                    case 1:  // 保存当前数据
                    {
                        cout << "请输入要保存的文件名（例如: data.txt）: ";
                        string filename;
                        getline(cin, filename);
                        if (filename.empty()) {
                            cout << "文件名不能为空！" << endl;
                            break;
                        }

                        try {
                            if (currentDataType == TYPE_INT) {
                                saveToFile(intNumbers, filename);
                            }
                            else if (currentDataType == TYPE_DOUBLE) {
                                saveToFile(doubleNumbers, filename);
                            }
                            else {
                                saveToFile(stringNumbers, filename);
                            }
                        }
                        catch (const SimpleSortException& e) {
                            cout << e.what() << endl;
                        }
                        break;
                    }

                    case 2:
                    {
                        cout << "请输入要加载的文件名: ";
                        string filename;
                        getline(cin, filename);
                        if (filename.empty()) {
                            cout << "文件名不能为空！" << endl;
                            break;
                        }

                        try {
                            if (currentDataType == TYPE_INT) {
                                intNumbers = loadFromFile<int>(filename);
                                if (!intNumbers.empty()) {
                                    cout << "\n加载的数据: ";
                                    outputNumbers(intNumbers);
                                }
                            }
                            else if (currentDataType == TYPE_DOUBLE) {
                                doubleNumbers = loadFromFile<double>(filename);
                                if (!doubleNumbers.empty()) {
                                    cout << "\n加载的数据: ";
                                    outputNumbers(doubleNumbers);
                                }
                            }
                            else {
                                stringNumbers = loadFromFile<string>(filename);
                                if (!stringNumbers.empty()) {
                                    cout << "\n加载的数据: ";
                                    outputNumbers(stringNumbers);
                                }
                            }
                        }
                        catch (const SimpleSortException& e) {
                            cout << e.what() << endl;
                        }
                        break;
                    }

                    case 3:
                    {
                        cout << "请输入保存排序结果的文件名: ";
                        string filename;
                        getline(cin, filename);
                        if (filename.empty()) {
                            cout << "文件名不能为空！" << endl;
                            break;
                        }

                        string sortedFilename = filename;
                        size_t dotPos = sortedFilename.find_last_of('.');
                        if (dotPos != string::npos) {
                            sortedFilename.insert(dotPos, "_sorted");
                        }
                        else {
                            sortedFilename += "_sorted";
                        }

                        try {
                            if (currentDataType == TYPE_INT && !intNumbers.empty()) {
                                saveToFile(intNumbers, sortedFilename);
                            }
                            else if (currentDataType == TYPE_DOUBLE && !doubleNumbers.empty()) {
                                saveToFile(doubleNumbers, sortedFilename);
                            }
                            else if (currentDataType == TYPE_STRING && !stringNumbers.empty()) {
                                saveToFile(stringNumbers, sortedFilename);
                            }
                            else {
                                cout << "没有数据可保存！" << endl;
                            }
                        }
                        catch (const SimpleSortException& e) {
                            cout << e.what() << endl;
                        }
                        break;
                    }
                    case 0:
                        cout << "返回主菜单..." << endl;
                        break;

                    default:
                        cout << "无效选择！" << endl;
                    }

                } while (fileChoice != 0);
                break;
            }

            case 6: showAlgorithmComplexity(); break;

            case 0:
                cout << "感谢使用，再见！" << endl;
                break;

            default:
                cout << "无效选择，请输入0-7之间的数字！" << endl;
            }

        }
        catch (const SimpleSortException& e) {
            cout << e.what() << endl;
        }
        catch (const exception& e) {
            cout << "系统错误: " << e.what() << endl;
        }
        catch (...) {
            cout << "未知错误发生" << endl;
        }

        try {
            autoSave();
        }
        catch (...) {
            // 保存失败不影响主流程
        }

    } while (mainChoice != 0);

    // 确保关闭可视化窗口
    closeVisualizationWindow();
    cout << "按回车退出..." << endl;
    cin.get();
    return 0;
}