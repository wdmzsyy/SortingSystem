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
using namespace std;
enum DataType { TYPE_INT, TYPE_DOUBLE, TYPE_STRING };
DataType currentDataType = TYPE_INT;    //默认一下
enum SortOrder { ORDER_ASC, ORDER_DESC };  // 升序，降序
SortOrder currentOrder = ORDER_ASC;

void mainMenu() {
    cout << "\n===== 排序系统 =====" << endl;
    cout << "当前数据类型: " << (currentDataType == TYPE_INT ? "整数" : (currentDataType == TYPE_DOUBLE ? "浮点数" : "字符串")) << endl;
    cout << "1.选择数据类型" << endl;
    cout << "2.输入数据" << endl;
    cout << "3.选择排序算法并执行" << endl;
    cout << "4.显示当前数据" << endl;
    cout << "0.退出系统" << endl;
    cout << "请选择: ";
}

void sortingMenu() {
    cout << "\n===== 选择排序算法 =====" << endl;
    cout << "1.冒泡排序（稳定）" << endl;
    cout << "2.插入排序（稳定）" << endl;
    cout << "3.选择排序（不稳定）" << endl;
    cout << "4.快速排序（不稳定）" << endl;
    cout << "5.归并排序（稳定）" << endl;
    cout << "0.返回上级菜单" << endl;
    cout << "请选择: ";
}

void selectDataType() {
    cout << "\n===== 选择数据类型 =====" << endl;
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
    const int MIN_INT = numeric_limits<int>::min();  // -2147483648
    const int MAX_INT = numeric_limits<int>::max();  // 2147483647
    if (value < MIN_INT || value > MAX_INT) {
        cout << "警告：整数 " << value << " 可能超出处理范围" << endl;
        return false;
    }
    return true;
}

bool validateDouble(double value) {
    // 检查是否是特殊值：无穷大(inf)或非数字(nan)
    if (!isfinite(value)) {
        cout << "错误：浮点数 " << value << " 不是有效数值（可能是inf或nan）" << endl;
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
void bubbleSort(vector<T>& numbers, SortOrder order = ORDER_ASC) {	//引用传递比值传递好，只用传地址
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
                if (numbers[j] > numbers[selectedIndex]) {  // 这里改了！
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

int main()	//放最后,就先不写声明了
{
    srand(time(nullptr));
    cout << "欢迎使用排序系统（支持整数/浮点数/字符串）" << endl;
    vector<int> intNumbers;
    vector<double> doubleNumbers;
    vector<string> stringNumbers;

    int mainChoice = 0;
    do {
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
        case 1: selectDataType(); break;
        case 2:
            cout << "\n--- 输入数据 ---" << endl;
            cout << "当前数据类型: " << (currentDataType == TYPE_INT ? "整数" :
                (currentDataType == TYPE_DOUBLE ? "浮点数" : "字符串")) << endl;
            if (currentDataType == TYPE_INT) {
                intNumbers = inputNumbers<int>();
            }
            else if (currentDataType == TYPE_DOUBLE) {
                doubleNumbers = inputNumbers<double>();
            }
            else {
                stringNumbers = inputNumbers<string>();
            }
            break;
        case 3:
        {
            if ((currentDataType == TYPE_INT && intNumbers.empty()) || (currentDataType == TYPE_DOUBLE && doubleNumbers.empty()) || (currentDataType == TYPE_STRING && stringNumbers.empty())) {
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

            SortOrder currentOrder = ORDER_ASC;  // 默认升序
            cout << "\n===== 选择排序方向 =====" << endl;
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

            cout << "\n排序前：";
            if (currentDataType == TYPE_INT) {
                outputNumbers(intNumbers);

                long long sortTime = 0;
                auto startTime = chrono::high_resolution_clock::now();
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
                cout << "排序后：";
                outputNumbers(intNumbers);
                auto endTime = chrono::high_resolution_clock::now();
                sortTime = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
                cout << "性能统计：排序耗时 " << sortTime << " 微秒" << endl;
                break;
            }
            else if (currentDataType == TYPE_DOUBLE) {
                outputNumbers(doubleNumbers);
                long long sortTime = 0;
                auto startTime = chrono::high_resolution_clock::now();
                cout << "正在使用";
                switch (sortChoice) {
                case 1: cout << "冒泡排序（稳定）"; bubbleSort(doubleNumbers, currentOrder); break;
                case 2: cout << "插入排序（稳定）"; insertionSort(doubleNumbers, currentOrder); break;
                case 3: cout << "选择排序（不稳定）"; selectionSort(doubleNumbers, currentOrder); break;
                case 4: cout << "快速排序（不稳定）"; quickSort(doubleNumbers, currentOrder); break;
                case 5: cout << "归并排序（稳定）"; mergeSort(doubleNumbers, currentOrder); break;
                numbersToSortdefault: cout << "无效选择"; continue;
                }
                cout << " 进行排序..." << endl;
                cout << "排序后: ";
                outputNumbers(doubleNumbers);
                auto endTime = chrono::high_resolution_clock::now();
                sortTime = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
                cout << "性能统计：排序耗时 " << sortTime << " 微秒" << endl;
                break;
            }
            else {
                outputNumbers(stringNumbers);
                long long sortTime = 0;
                auto startTime = chrono::high_resolution_clock::now();
                cout << "正在使用";
                switch (sortChoice) {
                case 1: cout << "冒泡排序（稳定）"; bubbleSort(stringNumbers, currentOrder); break;
                case 2: cout << "插入排序（稳定）"; insertionSort(stringNumbers, currentOrder); break;
                case 3: cout << "选择排序（不稳定）"; selectionSort(stringNumbers, currentOrder); break;
                case 4: cout << "快速排序（不稳定）"; quickSort(stringNumbers, currentOrder); break;
                case 5: cout << "归并排序（稳定）"; mergeSort(stringNumbers, currentOrder); break;
                default: cout << "无效选择"; continue;
                }
                cout << "进行排序……" << endl;
                cout << "排序后：";
                outputNumbers(stringNumbers);
                auto endTime = chrono::high_resolution_clock::now();
                sortTime = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
                cout << "性能统计：排序耗时 " << sortTime << " 微秒" << endl;
                break;
            }
        }
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
        case 0:
            cout << "感谢使用，再见！" << endl;
            break;
        default: cout << "无效选择，请重新输入！" << endl;
        }
    } while (mainChoice != 0);
    cout << "按回车退出..." << endl;
    cin.get();
    return 0;
}