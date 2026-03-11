#include <iostream>
#include <string>
using namespace std;

// Базовый класс 
class Film {
protected:
    string title;
    string genre;
    string description;

private:
    static int objectCount;

public:
    Film() {
        title = "Без названия";
        genre = "Не указан";
        description = "Нет описания";
        objectCount++;
    }

    Film(string t, string g, string d) {
        title = t;
        genre = g;
        description = d;
        objectCount++;
    }

    virtual ~Film() { //деструктор 
        objectCount--;
    }

    // сравнение фильмов по названию
    bool operator<(const Film& other) const {
        return title < other.title;
    }

    bool operator>(const Film& other) const {
        return title > other.title;
    }

    static int getObjectCount() {
        return objectCount;
    }

    virtual void display() const = 0;
};

int Film::objectCount = 0;


// Documentary 
class Documentary : public Film {
private:
    string topic;

public:
    Documentary() : Film() {
        topic = "Не указана";
    }

    Documentary(string t, string g, string d, string top)
        : Film(t, g, d) {
        topic = top;
    }

    void display() const override {
        cout << "Документальный: " << title << endl;
    }
};

//  Animation 
class Animation : public Film {
private:
    string animationType;

public:
    Animation() : Film() {
        animationType = "Не указан";
    }

    Animation(string t, string g, string d, string type)
        : Film(t, g, d) {
        animationType = type;
    }

    void display() const override {
        cout << "Анимация: " << title << endl;
    }
};



// шаблон класса
template <class T>
class ArrayTemplate {
private:
    T* data;
    int size;
    int count;

public:
    ArrayTemplate(int n) {
        size = n;
        count = 0;
        data = new T[size];
    }

    void add(T value) {
        if (count < size) {
            data[count] = value;
            count++;
        }
    }

    T get(int index) {
        return data[index];
    }

    T min() {
        T m = data[0];
        for (int i = 1; i < count; i++) {
            if (data[i] < m)
                m = data[i];
        }
        return m;
    }

    T max() {
        T m = data[0];
        for (int i = 1; i < count; i++) {
            if (data[i] > m)
                m = data[i];
        }
        return m;
    }

    int getCount() {
        return count;
    }
};





int main() {

    cout << "=== Шаблон с int ===" << endl;
    ArrayTemplate<int> intArr(5);
    intArr.add(10);
    intArr.add(3);
    intArr.add(25);
    cout << "Min: " << intArr.min() << endl;
    cout << "Max: " << intArr.max() << endl;


    cout << "\n=== Шаблон с char ===" << endl;
    ArrayTemplate<char> charArr(5);
    charArr.add('z');
    charArr.add('a');
    charArr.add('m');
    cout << "Min: " << charArr.min() << endl;
    cout << "Max: " << charArr.max() << endl;


    cout << "\n=== Шаблон с Documentary ===" << endl;
    ArrayTemplate<Documentary> docArr(3);
    docArr.add(Documentary("Океаны","","",""));
    docArr.add(Documentary("Африка","","",""));
    docArr.add(Documentary("Космос","","",""));

    cout << "Min: ";
    docArr.min().display();

    cout << "Max: ";
    docArr.max().display();


    cout << "\n=== Шаблон с Animation ===" << endl;
    ArrayTemplate<Animation> animArr(3);
    animArr.add(Animation("Шрек","","",""));
    animArr.add(Animation("Аладдин","","",""));
    animArr.add(Animation("Тачки","","",""));

    cout << "Min: ";
    animArr.min().display();

    cout << "Max: ";
    animArr.max().display();


    cout << "\n=== Шаблон с указателями на Film ===" << endl;
    ArrayTemplate<Film*> filmPtrArr(3);

    filmPtrArr.add(new Documentary("Земля","","",""));
    filmPtrArr.add(new Documentary("Космос","","",""));
    filmPtrArr.add(new Documentary("Африка","","",""));

    Film* minFilm = filmPtrArr.min();
    Film* maxFilm = filmPtrArr.max();

    cout << "Min: ";
    minFilm->display();

    cout << "Max: ";
    maxFilm->display();

    return 0;
}