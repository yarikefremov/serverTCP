#include "../include/dbtxt.h"

// Вывод строки без использования преобразования в std::string
void printString(const char* str){
    for(int i = 0; *(str+i) != char(0); ++i)
        std::cout<<*(str+i);
    std::cout<<std::endl;
}

DBTxt::DBTxt (const std::string &filename)
{

  // Копируем имя в атрибут класса и потом открываем файл c помощью fstream
  // причем только для чтения
  this->m_filename = filename;
  std::fstream s{ filename, s.in };

  // Если не удалось открыть файл, прекращаем работу
  if (!s.is_open ())
    {
      std::cout << "failed to open " << filename << '\n';
      s.close ();
      return;
    }

  // Создаем буфер, куда записываем name и password
  // Предполагается что длина пароля и имени - не более 32 символов, не считая
  // \0, Соответственно первые 33 байта - имя, вторые - пароль
  // buf - указатель на имя, buf+33 - указатель на пароль
  char buf[33 * 2];

  // Вплоть до конца файла построчно получаем имена и пароли
  // Файл должен быть организован как
  // имя1 пароль1
  // имя2 пароль2
  // имена с пробелами не разрешены
  while (!s.eof ())
    {

      // Получаем имя (все до пробела)
      s.getline (buf, 33, ' ');

      // Если имя пустое (скорее всего начало новой строки), то пользователь не
      // записывается
      if (buf[0] == 0)
        continue;

      // Получаем пароль, если он больше 33 символов, то программа запишет
      // только 33
      s.getline (buf + 33, 33);

      // Заполняем таблицу [имя] = пароль
      m_users[buf] = buf + 33;

      // Отладочная информация, в будущем можно будет убрать
      printString (buf);
      printString (buf + 33);
    }

  // Не забываем закрыть файл
  s.close ();
}

bool DBTxt::registerUser(const std::string& username, const std::string& password){

  // 1 возвращается только в случае, если пользователя нет в базе
  if (m_users.find (username) != 0)
    return 0;

  // Заполняем таблицу
  m_users[username] = password;
  return 1;
}

bool DBTxt::loginUser(const std::string& username, const std::string& password){

  // Функция возвращает 1 только если пользователь найден и пароли совпадают
  // std::unordered_map<std::string, std::string>::iterator
  auto a = m_users.find (username);

  // Пользователь не найден
  if (a == 0)
    return 0;

  // Пароли не совпадают
  if (password != a->second)
    return 0;

  // Все хорошо
  return 1;
}

DBTxt::~DBTxt(){

  // В деструкторе наша задача записать всю таблицу в файл, потом его закрыть

  // Открываем с перезаписью
  std::fstream s{ m_filename, s.out | s.trunc };

  // Поздравляем, не получилось, все данные в табличке не сохранены
  if (!s.is_open ())
    std::cout << "failed to open " << m_filename << '\n';

  // Если получается закрыть файл
  else
    {

      // Записываем в файл в виде
      // имя1 пароль1
      // имя2 пароль2
      // в конце обязательно будет \n
      for (const std::pair<const std::string, std::string> &el : m_users)
        s << el.first << ' ' << el.second << std::endl;
    }

  // Очищаем табличку, закрываем файл
  m_users.clear ();
  s.close ();
}
