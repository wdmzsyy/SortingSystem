#include<iostream>
#include<vector>
#include<string>    //getline函数需要
#include<sstream>   //添加sstream用于字符串分隔
#include<cstdlib>   //rand()
#include<ctime>
using namespace std;
//测试提交功能
enum DataType {TYPE_INT, TYPE_DOUBLE};
DataType currentDataType = TYPE_INT;    //默认一下

void mainMenu() {
    cout << "\n===== 排序系统 =====" << endl;
    cout << "当前数据类型: " << (currentDataType == TYPE_INT ? "整数" : "浮点数") << endl;
    cout << "1.选择数据类型" << endl;
    cout << "2.输入数据" << endl;
    cout << "3.选择排序算法并执行" << endl;
    cout << "4.显示当前数据" << endl;
    cout << "0.退出系统" << endl;
    cout << "请选择: ";
}

void sortingMenu() {
    cout << "\n===== 选择排序算法 =====" << endl;
    cout << "1.冒泡排序" << endl;
    cout << "2.插入排序" << endl;
    cout << "3.选择排序" << endl;
    cout << "4.快速排序" << endl;
    cout << "5.归并排序" << endl;
    cout << "0.返回上级菜单" << endl;
    cout << "请选择: ";
}

void selectDataType() {
    cout << "\n===== 选择数据类型 =====" << endl;
    cout << "1.整数（int）" << endl;
    cout << "2.浮点数（double）" << endl;
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
    else {
        cout << "无效选择，保持当前类型" << endl;
    }
}

//输入数据
template<typename T>
vector<T> inputNumbers() {
    cout << "请输入" << (typeid(T) == typeid(int) ? "整数":"浮点数")
         << "（空格分隔，回车结束）: ";
    vector<T> nums;  // 创建一个空数组
    string line;
    getline(cin, line); //读取一整行，包含回车
    stringstream ss(line);//使用字符串流解析数字
    T num{};
    while (ss >> num) {       //从字符串流中读取数字
        nums.push_back(num);  // 添加到数组
    }
    if (nums.empty()) {
        cout << "警告：没有输入任何数据！！！" << endl;
    }
    else {
        cout << "成功输入" << nums.size() << "个数据" << endl;
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
void bubbleSort(vector<T>& numbers) {	//引用传递比值传递好，只用传地址
    int n = numbers.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (numbers[j] > numbers[j + 1]) {
                int temp = numbers[j];
                numbers[j] = numbers[j + 1];
                numbers[j + 1] = temp;
            }
        }
    }
}

template<typename T>
void insertionSort(vector<T>& numbers) {
    int n = numbers.size();
    for (int i = 1; i < n; i++) {
        T key = numbers[i];   //i从第二个数开始，key是当前的数值
        int j = i - 1;
        while (j >= 0 && numbers[j] > key) {
            numbers[j + 1] = numbers[j];
            j--;
        }
        numbers[j + 1] = key;
    }
}

template<typename T>
void selectionSort(vector<T>& numbers) {
    int n = numbers.size();
    for (int i = 0; i < n - 1; i++) {
        // 找到[i, n-1]区间内最小元素的索引
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            if (numbers[j] < numbers[minIndex]) {
                minIndex = j;
            }
        }
        // 将最小元素交换到位置i
        if (minIndex != i) {
            swap(numbers[i], numbers[minIndex]);
        }
    }
}

template<typename T>
int partition(vector<T>& numbers, int low, int high) {
    // 随机选择基准值
    int randomIndex = low + rand() % (high - low + 1);
    swap(numbers[low], numbers[randomIndex]);

    T pivot = numbers[low];

    while (low < high) {
        while (low < high && numbers[high] >= pivot) --high;
        numbers[low] = numbers[high];

        while (low < high && numbers[low] <= pivot) ++low;
        numbers[high] = numbers[low];
    }

    numbers[low] = pivot;
    return low;
}
template<typename T>
void quickSort(vector<T>& numbers, int low = 0, int high = -1) {
    if (high == -1) high = numbers.size() - 1;

    while (low < high && !numbers.empty()) {
        int pivotPos = partition(numbers, low, high);

        // 总是先递归处理较短的部分
        if (pivotPos - low < high - pivotPos) {
            quickSort(numbers, low, pivotPos - 1);
            low = pivotPos + 1;  // 处理长部分（转换为循环）
        }
        else {
            quickSort(numbers, pivotPos + 1, high);
            high = pivotPos - 1;  // 处理长部分（转换为循环）
        }
    }
}

template<typename T>
void merge(vector<T>& arr, int left, int mid, int right) {
    // 创建临时数组
    vector<T> temp(right - left + 1);
    int i = left;      // 左半部分起始索引
    int j = mid + 1;   // 右半部分起始索引
    int k = 0;         // 临时数组索引

    // 合并两个有序数组
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        }
        else {
            temp[k++] = arr[j++];
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
void mergeSort(vector<T>& arr, int left = 0, int right = -1) {
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
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    // 合并两个有序部分
    merge(arr, left, mid, right);
}

int main()	//放最后,就先不写声明了
{
    srand(time(nullptr));
    cout << "欢迎使用排序系统（支持整数/浮点数）" << endl;
    vector<int> intNumbers;      // 存储整数数据
    vector<double> doubleNumbers; // 存储浮点数数据
  
    int mainChoice = 0;
    do {
        mainMenu();
        cin >> mainChoice;
        cin.ignore();
        switch (mainChoice) {
        case 1: selectDataType(); break;
        case 2: 
            if (currentDataType == TYPE_INT) { intNumbers = inputNumbers<int>(); }
            else { doubleNumbers = inputNumbers<double>(); }
            break;
        case 3:
        {
            if ((currentDataType == TYPE_INT && intNumbers.empty()) || (currentDataType == TYPE_DOUBLE && doubleNumbers.empty())) {
                cout << "错误！请先输入数据！" << endl;
                break;
            }
            int sortChoice = 0;
            sortingMenu();
            cin >> sortChoice;
            cin.ignore();
            if (sortChoice == 0)break;
            cout << "\n排序前：";
            if (currentDataType == TYPE_INT) {
                outputNumbers(intNumbers);
                vector<int>numbersToSort = intNumbers;
                cout << "正在使用";
                switch (sortChoice) {
                case 1: cout << "冒泡排序"; bubbleSort(numbersToSort); break;
                case 2: cout << "插入排序"; insertionSort(numbersToSort); break;
                case 3: cout << "选择排序"; selectionSort(numbersToSort); break;
                case 4: cout << "快速排序"; quickSort(numbersToSort); break;
                case 5: cout << "归并排序"; mergeSort(numbersToSort); break;
                default: cout << "无效选择"; continue;
                }
                cout << "进行排序……" << endl;
                cout << "排序后：";
                outputNumbers(numbersToSort);
                break;
            }
            else {
                outputNumbers(doubleNumbers);
                vector<double> numbersToSort = doubleNumbers;
                cout << "正在使用";
                switch (sortChoice) {
                case 1: cout << "冒泡排序"; bubbleSort(numbersToSort); break;
                case 2: cout << "插入排序"; insertionSort(numbersToSort); break;
                case 3: cout << "选择排序"; selectionSort(numbersToSort); break;
                case 4: cout << "快速排序"; quickSort(numbersToSort); break;
                case 5: cout << "归并排序"; mergeSort(numbersToSort); break;
                default: cout << "无效选择"; continue;
                }
                cout << " 进行排序..." << endl;
                cout << "排序后: ";
                outputNumbers(numbersToSort);
                break;
            }
        }
        case 4: 
            cout << "\n当前数据：";
            if (currentDataType == TYPE_INT) {
                outputNumbers(intNumbers);
            }
            else {
                outputNumbers(doubleNumbers);
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