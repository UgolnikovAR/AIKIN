# AIKIN
AIKIN repository.


При росте качества вычислительных ресурсов, все больше растет запрос качества на мультимедийные продукты. Например, обработка видео в высоком разрешении может занимать часы времени даже на самом современном оборудовании. Например, компания Дисней использует для обработки своих картин сотни различных компьютеров. Такие средства есть у ведущих производителей мультимедии.

Идея проекта заключается в том, чтобы реализовать возможность делегирования объемных ресурсоемких вычислительных задач удаленным машинам, а также одновременное выполнение параллельных вычислений на разных машинах.
Нужна база – формализованная система, которая позволит создавать и выполнять такие процессы и управлять ими – распределять вычислительную нагрузку. Сами задачи должны быть предельно формализованными и будут делиться на три уровня: клиент – сервер – вычислительный агент. Это будет достигаться за счет размещения динамических библиотек в программах системы, в которых будут располагаться необходимые средства.

Доступ к ресурсам ЭВМ будет достигаться за счет установки специальной программы – вычислительного агента на устройство. Эта программа должна быть ограниченна в плане задействования ресурсов; должна быть возможность переопределения допустимой доли задействования ресурсов ЭВМ. 

Распределение нагрузки организует специальный менеджер процессов на уровне сервера. Такой модуль отслеживает показатели вычислительной активности агентов и распределяет вычислительную нагрузку в виде дискретных заданий, которые поступают агентам. Здесь необходимо также организовать защиту от перенасыщения отдельных агентов нагрузкой.

Само вычислительное задание и его определение задается на уровне клиента и поступает серверу. Клиент должен иметь возможность быть информированным о характеристиках протекающих процессов. Необходимо организовать сбор статистики от агентов. Таким образом с точки зрения клиента вся система представляет собой один единственный виртуальный вычислительный аппарат.


# Стандарты стилизации кода и именования переменных
Смысловой блок кода - это последовательно идущие друг за другом строки не разделенные пустыми строками. Иногда могут допускаться разделения пустыми строками. В этом случае необходимо обращать внимание на комментарии вида /*.. ..*/.

Комментарии вида /*.. ..*/(многострочный) - используются для местных примечаний и пояснений прототипов, объявлений, а так же непосредственно в коде. С него начинается смысловой блок. Такое пояснение относится ко всему следующему смысловому блоку кода. Если такой комментарий стоит перед  прототипом или объявлением, то он относится только к одному следующему прототипу или объявлению. Иногда допускаю разделение смысловых блоков пустыми строками. В этом случае нужно обращать внимание на следующий многострочный комментарий.

Комментарии вида //(однострочный) - относятся к следующему, за ним, оператору. Такой комментарий может применяться непосредственно в строке оператора. В этом случае он относится к той строке оператора, в которой находится.

Названия классов – UpperCamelCase.

Название открытых методов lowerCamelCase.

Имена snake_low_case – относятся к закрытым методам класса.

В целом, в именовании переменных, я придерживаюсь венгерской нотации. В основном это касается членов классов.

Приставка _ (нижнее подчеркивание) – указывает на то, что следующий символ (имя) является именем закрытого члена текущего объекта. Подразумевается, что члены с такими именами должны использоваться исключительно в рамках текущего класса. Перед ним может быть сокращенная запись, отображающая происхождение этого ОД. 

В общем случае имена полей имеют следующий вид: <происхождение?>_<указатель?><название>. Например, sma_pspotter – static main agent pointer to spotter – закрытый член класса, указатель на объект spotter, который является статической частью agent. Или _psocket – закрытый член класса, указатель на сокет.

