// cppl-diplom.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <map>
#include <sstream>


class ini_parser {
private:
    ini_parser() = delete;

    //буфера для запоминания значений в виде строк для заполнения контейнера класса:
    //буфер строки считанной с файла, буфер имени секции, буфер имени параметра, буфер значения параметра соответственно
    std::string buff_main, buff_section, buff_first, buff_second;
    //основной вектор, где будем хранить отсортированные данные из файла.
    //структура: 
    // основной map содержащий строку(имя секции) и доп.map (с имя параметра, значение параметра)
    std::map<std::string, std::map<std::string, std::string>> main_vec;


public:
    // конструктор 
    ini_parser(std::string filename) {
        //ооткрываем файл для чтения
        std::ifstream reader(filename);
        //проверка, если не получилось, бросаем исключение
        if (!reader.is_open()) {
            throw std::runtime_error("Unable to read file!");
        }
        //считываем построчно (игнорируя пробелы и табы в начале) до конца файла и записываем данные из 
        // файла в контейнер, предварительно их разграничив
        while (std::getline(reader >> std::ws,buff_main)) {

            //если в начале строки ";", то пропускаем эту строку
            if (buff_main[0] == ';') { continue; }
            //если на новой строке нет символа в начале строки, то пропускаем
            if (buff_main[0] == '\n' || buff_main.length() == 0) { continue; }

            //если видим квадратную скобку, то это имя секции, ее запомним в отдельный буфер и 
            // считываем след строку с файла без квадратных скобок
            if (buff_main[0] == '[') {
                buff_section = buff_main.substr(1,buff_main.length()-2);
                //std::cout << buff_section << std::endl;
            }
            //если это не ";" и не имя секции, то разбираемся с этой строкой, выделяя имя переменной и ее значение
            else {
                //ищем в строке ";", чтобы отсечь комментарий к отдельному параметру и запоминаем позицию ";", если есть
                size_t semicolon = buff_main.find(';');
                if (semicolon == -1) { semicolon = buff_main.length(); }
                //запоминаем расположение символа "="
                size_t ravno = buff_main.find('=');

                //пишем в буфер имя параметра
                buff_first = buff_main.substr(0, ravno);
                //значение параметра
                buff_second = buff_main.substr(ravno + 1, semicolon - ravno - 1);

                //после получения всех нужых данных мы заполняем основной контейнер класса

                //std::vector<std::pair<std::string, std::string>> buff_vec; //буферный вектор
                //buff_vec.push_back(std::make_pair(buff_first, buff_second));//пишем в буферный вектор данные

                //смотрим, есть ли уже такое название секции есть, то информацию о параметре нужно будет писать в вектор
                // принадлежащий этой секции 
                //bool if_new_section = true;//меняем на фалс, если секция уже есть
                //bool if_new_parameter = true; //меняем на фалс, если параметр уже есть
                //for (auto& x : main_vec) {
                //    if (x.first == buff_section) {
                //        //если секция такая уже есть, то меняем флаг и идем по вектору, принадлежащему этой секции
                //        if_new_section = false;
                //        for (auto& y : x.second) {
                //            //если уже есть имя параметра, то меняем флаг и перезаписываем значение параметра
                //            if (y.first == buff_first) {
                //                if_new_parameter = false;
                //                y.second = buff_second;
                //            }
                //            
                //        }
                //        //если имени параметра нет, то добавляем в вектор новую пару: имя парам-ра и его значение
                //        if(if_new_parameter){
                //            x.second.push_back(std::make_pair(buff_first, buff_second));
                //        }
                //    }
                //}
                ////если секция новая, то добавляем в основной вектор пару: имя секции и вектор с парами (имя пар-ра и знач) 
                //if (if_new_section) {
                //    main_vec.push_back(std::make_pair(buff_section, buff_vec));
                //}

                //для проверки, если новая секция (если уже есть, то меняем на фалс)
                bool if_new_section = true;
                //проверяем, если название сектора уже есть
                for (auto& x : main_vec) {
                    //если есть, то меняем флаг на фалс
                    if (x.first == buff_section) {
                        if_new_section = false;
                        //и записываем в контейнер, привязанный к этому сектору имя параметра и его значение
                        x.second[buff_first] = buff_second;
                        
                    }
                }

                //если флаг не меняли(нет такой секции)
                //добавляем в контейнер название секции и пару(имя параметра и его значение)
                if (if_new_section) {
                    std::map<std::string,std::string> buff_map;
                    buff_map[buff_first] = buff_second;
                    main_vec.emplace(buff_section, buff_map);
                }
            }
            //std::cout << '"' << buff_main << '"' << " added." << std::endl;
        }
        //закрываем 
        reader.close();
    }

    //выводит содержимое основного map класса в консоль
    void heavy_print() {

        for (auto& x : main_vec) {
            std::cout << std::endl << "Секция: " << x.first << std::endl;
            for (auto& y : x.second) {
                std::cout << "Параметр: " << y.first << " Значение: " << y.second << std::endl;
            }
        }

    }
private:
    
    //проверяет правильность запроса юзера и возвращает значение параметра, если успех 
    std::string find_or_not(std::string& user_input) {
        //находим точку, разделяющую название секции и имя параметра
        size_t dot = user_input.find('.');
        if (dot == -1) {
            throw std::invalid_argument("Неправильный формат ввода! Правильно: \"Section.parameter\"");
        }
        std::string sect = user_input.substr(0, dot);//имя секции
        std::string param = user_input.substr(dot + 1);//имя параметра

        //для удобства пишем в буфер содержимое параметра, предварительно проверяя инпут юзера
        std::string buff;
        //если находим ключ такой же, как инпут юзера в имени секции
        auto sect_search = main_vec.find(sect);
        if (sect_search != main_vec.end()) {
            //то проверяем совпадение по ключу в именах параметров этой секции
            auto param_search = main_vec[sect].find(param);
            if (param_search != main_vec[sect].end()) {
                //если нашли совпадение по имени параметра, то пишем в буфер содержимое контейнера
                buff = main_vec[sect][param];
            }
            //если нет такого имени параметра, то бросаем исключение
            else {
                std::string err_text = "Неверное имя параметра!\nЕсть такие наименования параметров в секции '" + sect + "': " + "\n";
                for (auto& x : main_vec[sect]) {
                    err_text += x.first + "\n";
                }
                throw std::invalid_argument(err_text);
            }
        }
        //если нет, то бросаем исключение
        else {
            std::string err_text = "Неверное имя секции!\nЕсть такие наименования секций:\n";
            for (auto& x : main_vec) {
                err_text += x.first + "\n";
            }
            throw std::invalid_argument(err_text);
        }
        //если секция и имя параметра было указано верно, но в строке нет ничего, значит там неьт значения
        if (buff.length() == 0) {
            throw std::invalid_argument("Нет значения для этого параметра");
        }
        return buff;
    }

    //распознает, какой тип данных по идее хранится в строке и возвращает:
    //0-строка, 1-дабл, 2-инт
    int check_data_type(std::string& value) {
        //проверяем значение параметра на принадлежность числу или строке
        int status = 0;
        //если в значении есть точка, то это либо дабл (status = 1), либо строка (0)
        //если ее нет, то это либо инт (2), либо строка (0)
        auto dot_search = value.find('.');
        //если точка есть
        if (dot_search != -1) {
            //пробуем преобразовать значение параметра в дабл
            try {
                auto check = std::stod(value);
                //return check;
                status = 1;
                //std::cout << status << "double";

            }
            //если не выходит, значит это строка, ловим исключение, чтобы не прерывать программу
            catch (std::invalid_argument&) {
                //status = 0;
            }
        }
        //если точки нет
        else {
            //то пробуем превратить в инт
            try {
                auto check = std::stoi(value);
                //return check;
                status = 2;
                //std::cout << status << "int";
            }
            //если не получается, то это строка --  ловим исключение и идем дальше
            catch (std::invalid_argument&) {

            }
        }
        return status;
    }
   
    //проверяет соотвествие имеющегося типа в значении параметра с запрашиваемым юзером
    //в случае несоответсвия выкидывает исключение
    template<class T>
    void check_correct_type_asked(int status) {
        switch (status)
        {
            //если у нас строка, а запрос не по ней
        case 0:
            if (typeid(T) != typeid(std::string)) {
                throw std::invalid_argument("Запрошен неверный тип данных параметра! Там лежит std::string");
            }
            break;
            //если у нас дабл, а запрос не по нему
        case 1:
            if (typeid(T) != typeid(double)) {
                throw std::invalid_argument("Запрошен неверный тип данных параметра! Там лежит Double");
            }
            break;
            //если у нас инт, а запрос не по нему
        case 2:
            if (typeid(T) != typeid(int)) {
                throw std::invalid_argument("Запрошен неверный тип данных параметра! Там лежит Int");
            }
            break;
        }
    }

    //для возвращения уже проверенного ответа запрошенного типа
    template<class T>
    auto answer(std::string value) {
        return value;
    }
    template<int>
    int answer(std::string value) {
        int ans = std::stoi(value);
        return ans;
    }
    template<double>
    double answer(std::string value) {
        double ans = std::stod(value);
        return ans;
    }





public:
    template <class T>
    auto get_value(std::string user_input) {
        //проверяем ввод юзера и получаем значение по его запросу, если успех
        std::string value = find_or_not(user_input);
        
        //получаем понимание о предполагаемом типе данных
        //0-строка, 1-дабл, 2-инт
        int status = check_data_type(value);        
    
        //после того, как мы поняли, какое значение лежит в параметре, сравниваем с тем типом, что запрашивает юзер
        check_correct_type_asked<T>(status);        

        //и раз уж мы добрались сюда и не выбросили исключение, можно возвращать ответ
        return answer<T>(value);
    }
    
    /*~ini_parser() {
 
    }*/
};

int main()
{
    setlocale(LC_ALL, "rus");
    setlocale(LC_NUMERIC, "C");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    try {
        ini_parser p("for_parcer.ini");
        //p.heavy_print();
        auto val = p.get_value<double>("Section1.var1");
        std::cout << val;
        
    }
    catch (std::exception& er) {
        std::cout << er.what() << std::endl;
    }
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
