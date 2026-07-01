## Как я делал компактную библиотеку для создания приложений с графическим интерфейсом пользователя на языке C++. Часть-2 - Platform backend.

Данная статья является следующей в цикле статей о создании компактной кросс-платформенной библиотеки для разработки приложений с графическим интерфейсом на языке C++ - Frenchie. Для тех, кто привык изучать исхоный код самостоятельно, репозиторий с исходным кодом библиотеки можно найти по ссылке - **https://github.com/Alexey-cpu/Frenchie**. В прошлой статье мы реализовали статический класс, моделирующий цикл работы приложения с графическим интерфейсом. В этом материале я расскажу о том, как работает та часть библиотеки, которая отвечает за управление состоянием контекстного окна и отслеживания в нем событий - platform backend.

platform backend - эта та часть библиотеки, кототрая отвечает за управление состоянием контекстного окна и связанных с ним событий. В этой статье мы разберемся, что же делает platform backend в цикле работы приложения и как он реализован. Начать предлагаю с описания того, что platform backend делает в цикле работы приложения.Итак, при запуске приложения, platform backend открывает контекстное окно и запускает инициализацию используемого графического API при помощи rendering backend-а. Вначале каждого кадра platform backend кеширует размер контекстного окна, размер его frame buffer-а, а также события, связанные с контекстным окном: нажатие кнопок мыши, клавиатуры, ввод текста и.т.д. Далее, в конце кадра platform backend отображает в контекстном окне то, что нарисовала видеокарта, используя rendering backend, а также сбрасывает кеш событий контекстного окна. На этапе закрытия приложения, platform backend запускает деинициализацию графического API, используя rendering backend, а потом закрывает контекстное окно, когда деинициализация закончилась. Таким образом, скелет статического класса platfrom backend-а будет выглядеть вот так:

``` C++
namespace Frenchie
{
    namespace Application
    {
        class ApplicationPlatformBackend final
        {
        public:

            ApplicationPlatformBackend() = delete;
            ApplicationPlatformBackend(const ApplicationPlatformBackend&) = delete;
            ApplicationPlatformBackend& operator=(const ApplicationPlatformBackend&) = delete;

            // открывает контекстное окно и запускает процесс инициализации графического API
            static bool awake();

            // кеширует события, связанные с контекстным окном, а также его свойства: размер окна, размер frame buffer-а, положение
            static void frame_start();

            // эта функция в основном нужна для сброса фиксации текстового ввода
            static void frame_update();

            // отображает в контекстном окне то, что нарисовала видеокарта и сбрасывает кеш событий
            static void frame_finish();

            // закрывает контекстное окно и запускает деинициализацию rendering backend-а
            static void quit();

            // проверяет, нужно ли закрыть контекстное окно
            static bool is_closed();

            // отдает команду на закрытие контекстного окна
            static void close();

        private:

            // отслеживает события, связанные с контекстным окном, сохраняя их в кеш событий
            static void collect_input();

            // сбрасмывает кеш событий
            static void restore_input();
        };
    }
}
```

Итак, скелет мы подготовили, теперь возникает вопрос о том, что же такое кеш событий и где же он лежит ? Ранее мы говорили, что в кеше событий храниться информация о состоянии клавиш клавиатуры и мыши, положении курсора, о размерах контекстного окна и.т.д Все это вместе можно назвать состоянием platfom backend-а и хранится оно в соответствующей статической переменной статического класса platform backend-а:

``` C++
namespace Frenchie
{
    namespace Application
    {
        // состояние platform backend-а, хранящее в себе как минимум указатель на контекстное окно,
        // а также кеш связанных с ним событий
        struct FrenchieApplicationPlatformApi
        {
            FrenchieApplicationPlatformApi(){}
            virtual ~FrenchieApplicationPlatformApi(){}

            void*                           Window = nullptr; // указатель на контекстное окно
            ApplicationPlatformBackendInput Input;            // кеш событий контекстного окна
        };

        // класс platfrom backend-а
        class ApplicationPlatformBackend final
        {
        public:

            ApplicationPlatformBackend() = delete;
            ApplicationPlatformBackend(const ApplicationPlatformBackend&) = delete;
            ApplicationPlatformBackend& operator=(const ApplicationPlatformBackend&) = delete;

            // открывает контекстное окно и запускает процесс инициализации графического API
            static bool awake();

            // кеширует события, связанные с контекстным окном, а также его свойства: размер окна, размер frame buffer-а, положение
            static void frame_start();

            // эта функция в основном нужна для сброса фиксации текстового ввода
            static void frame_update();

            // отображает в контекстном окне то, что нарисовала видеокарта и сбрасывает кеш событий
            static void frame_finish();

            // закрывает контекстное окно и запускает деинициализацию rendering backend-а
            static void quit();

            // проверяет, нужно ли закрыть контекстное окно
            static bool is_closed();

            // отдает команду на закрытие контекстного окна
            static void close();

            // выдает состояние platform backend-а
            template <typename T = FrenchieApplicationPlatformApi>
            static std::shared_ptr<T> platform_api()
            {
                return std::dynamic_pointer_cast<T>(m_Api);
            }

        private:

            static std::shared_ptr<FrenchieApplicationPlatformApi> m_Api;

            // отслеживает события, связанные с контекстным окном, сохраняя их в кеш событий
            static void collect_input();

            // сбрасмывает кеш событий
            static void restore_input();
        };
    }
}
```

Из кода выше возникает вопрос о том, почему функция, выдающая состояние platform backend-а шаблонная ? Дело в том, что указатель на контекстное окно и кеш его событий - это минимальное количество информации, которое нужно хранить для успешной работы с контекстным окном. Помимо этого, состояние platform backend-а может хранить еще какие-то служебные переменные в зависимости от операционной системы и/или используемой сторонней библиотеки, являющейся слоем абстракции между низкоуровневыми функциями операционной системы и нашей библиотекой, но, об этом позже. А сейчас, узнав, где находится кеш событий контекстного окна, надо бы выяснить, что в нем лежит:

``` C++
namespace Frenchie
{
    namespace Application
    {
        struct ApplicationPlatformBackendInput
        {
            // кеш состояния кнопок мыши
            ApplicationPlatformBackendMouseButton  MouseButtons     [ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd]{};

            // кеш состояния курсора
            ApplicationPlatformBackendMouseCursor  MouseCursor      {ApplicationPlatformBackendMouseCursor()};

            // кеш состояния окна
            ApplicationPlatformBackendWindow       Window           {ApplicationPlatformBackendWindow()};

            // кеш состояния модификаторов кнопок мыши и клавиатуры
            ApplicationPlatformBackendKeyModifier  Modifiers        [ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_End]{};

            // кеш состояния кнопок клавиатуры
            ApplicationPlatformBackendKey          Keys             [ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_END]{};

            // кеш с воодимым символом
            Frenchie::Core::Optional<unsigned int> Character;

            // кеш с нормированным положением колесика мыши
            gs_vec2f                               MouseScrollOffset{gs_vec2f(0.f, 0.f)};

            // размер окна
            gs_vec2f                               WindowSize;

            // положение окна
            gs_vec2f                               WindowPosition;

            // размер frame buffer-а окна
            gs_vec2f                               FrameBufferSize;
        };
    }
}
```

Выше представлен кеш событий, связанный с контекстным окном и первое, что мы там видим - это вектор с состоянием кнопок мыши, который для каждой кнопки мыши хранит ее состояние, описываемое следующим образом:

``` C++
namespace Frenchie
{
    namespace Application
    {
        // состояние кнопки мыши
        struct ApplicationPlatformBackendMouseButton
        {
            // перечисление со всеми видами кнопок мыши
            enum Button : int
            {
                ApplicationPlatformBackendMouseButtonBegin,
                
                // Здесь и далее я буду опускать все значения перечисления кроме begin и end дабы не загромождать и без того больщую статью
                //...
                
                ApplicationPlatformBackendMouseButtonEnd
            };

            // количество нажатий на кнопку
            int                              Clicks       {0    };

            // определяет зажата кнопка или нет
            bool                             Down         {false};

            // определяет удерживалась ли кнопка в течение заданного времени
            bool                             Hold         {false};

            // определяет нажал пользователь кнопку или нет
            bool                             Pressed      {false};

            // определяет отпустил пользователь кнопку или нет
            bool                             Released     {false};

            // определяет скликнул пользователь кнопку или нет
            bool                             Clicked      {false};

            // определяет скликнул пользователь кнопку два раза или нет
            bool                             DoubleClicked{false};

            // момент времени нажатия на кнопку
            Frenchie::Core::Clock::TimePoint PressTime    {Frenchie::Core::Clock::TimePoint()};

            // момент времни, когда кнопку отпустили
            Frenchie::Core::Clock::TimePoint ReleaseTime  {Frenchie::Core::Clock::TimePoint()};
        };
    }
}
```

Из кода выше становится понятно, что ключевыми событиями для кнопки мыши являются нажатие, отпускание, зажатие и скиликивание. В чем же разница между этими событиями ? С нажатием, зажатием и отпусканием все очевидно: нажатие кнопки мыши - это событие, когда пользователь нажал на нее, отпускание - это когда пользователь отпустил ее, а зажатие - это когда пользователь удерживает кнопку в нажатом сотоянии (т.е ткнул в нее пальцем и держит). Самое интересное обстоит с зажатием и скликиванием. Зажатие кнопки - это событие, когда пользователь нажал на кнопку и держал ее в зажатом состоянии в течении заданного количества времени. Скликивание - это событие, когда пользователь нажал кнопку, подержал ее в нажатом состоянии какое-то время и потом отпустил. Как раз для фиксации такого рода событий нам и нужно фиксировать для каждой кнопки моменты времени нажатия и отпускания. С двойным скликиванием, думаю, все понятно: мы его определяем, посчитав количество скликиваний, которое также хранится у нас в состоянии кнопки. Теперь разберемся с описанием состояния кнопок клавиатуры. На самом деле, оно будет практически такое же, как и для кнопок мыши:

``` C++
namespace Frenchie
{
    namespace Application
    {
        // состояние кнопки клавиатуры
        struct ApplicationPlatformBackendKey
        {
            enum Key : int
            {
                ApplicationPlatformBackendKey_NamedKey_BEGIN = 0,

                // Здесь и далее я буду опускать все значения перечисления кроме begin и end дабы не загромождать и без того больщую статью
                //...

                ApplicationPlatformBackendKey_NamedKey_END
            };

            // количество нажатий на кнопку
            int                              Clicks       {0    };

            // определяет зажата кнопка или нет
            bool                             Down         {false};

            // определяет удерживалась ли кнопка в течение заданного времени
            bool                             Hold         {false};

            // определяет нажал пользователь кнопку или нет
            bool                             Pressed      {false};

            / определяет отпустил пользователь кнопку или нет
            bool                             Released     {false};

            // определяет скликнул пользователь кнопку или нет
            bool                             Clicked      {false};

            // момент времени нажатия на кнопку
            Frenchie::Core::Clock::TimePoint PressTime    {Frenchie::Core::Clock::TimePoint()};

            // момент времни, когда кнопку отпустили
            Frenchie::Core::Clock::TimePoint ReleaseTime  {Frenchie::Core::Clock::TimePoint()};
        };
    }
}
```

В какой-то момент времени я хотел объединить состояние для кнопок мыши и клавиатуры, но, как-то не задалось, поэтому эти два состояни теперь и описываются разными структурами. Перейдем теперь к модификаторам. Модификаторы определяются в основном зажатыми клавишами CTRL, ALT, SHIFT или COMAND на MacOS и у них одно единственное состояние - активен/неактивен:

``` C++
namespace Frenchie
{
    namespace Application
    {
        struct ApplicationPlatformBackendKeyModifier
        {
            // все возможные модификаторы
            enum Modifier : int
            {
                ApplicationPlatformBackendKeyModifier_Begin,
                // Здесь и далее я буду опускать все значения перечисления кроме begin и end дабы не загромождать и без того больщую статью
                //...
                ApplicationPlatformBackendKeyModifier_End,
            };

            // определяет активен модификатор сейчас или нет
            bool Active = false;
        };
    }
}
```

Хочется сразу заметить, что перечисления с клавишами клавиатуры, кнопками мыши и модификаторами в своей библиотеке я заимствовал из библиотеки ImGUI, дабы не писать их самостоятельно и внимательный читатель, который работал с ImGUI мог уже это заметить. Так, про клавиши клавиатуры, мыши и их модификаторы поговорили, теперь разберемся, какие переменные состояния есть у курсора мыши:

``` C++
namespace Frenchie
{
    namespace Application
    {
        // состояние курсора мыши
        struct ApplicationPlatformBackendMouseCursor
        {
            // опрелделяет вошел курсор в область контекстного окна или нет
            bool      Entered           {false};

            // позиция курсора мыши внутри контекстного окна
            gs_vec2f  Position          {gs_vec2f(0.f)};

            // позиция курсора мыши в момент нажатия
            gs_vec2f  MousePressPosition{gs_vec2f(0.f)};

            // перемещение курсора, когда мы 'тянем' его, т.е разница между позицией нажатия мыши и тем, где сейчас
            // находится курсор при зажатой кнопке мыши
            gs_vec2f  DragDelta         {gs_vec2f(0.f)};
        };
    }
}
```

У курсора мыши очевидно есть положение, но, самое интересное в нем - это переменная, описывающая перемещение при зажатии кнопки мыши. Для фиксации того, насколько мы перетащили курсор при зажатом состоянии любой кнопки мыши, мы должны зафиксировать положение курсора в момент нажатия кнопки мыши, а потом считать разницу между тем, где курсор был в момент нажатия и тем, где он находится сейчас до тех пор, пока пользователь не отпустит зажатую кнопку мыши. Вот, мы уже почти все и рассмотрели за исключением состояния самого контекстного окна:

``` C++
namespace Frenchie
{
    namespace Application
    {
        // состояние контестного окна
        struct ApplicationPlatformBackendWindow
        {
            // определяет, передала ли операционная система фокус нашему контекстному окну
            bool Focused {false};
        };
    }
}
```
У контекстного окна на данный момент имеется только переменная для фиксации факта передачи фокуса контекстному окну операционной системой. Это нужно, чтобы начать фиксировать события и сохранять их в кеш, в ином случае мы считаем, что окно не активно.

На данный момент мы разобрались с клавишами клавиатуры, мыши, модификаторами и фокусом контекстного окна, однако ничего не узнали о том, что же такое размер frame buffer-а контекстного окна и чем же он отличается от размера самого окна ? Размер frame buffer-а контекстного окна - это размер той области окна, в которой окно рисует то, что нарисовала нам видеокарта, т.е - это размер окна минус размер оконной рамки... Но, это только для случая, когда количество пикселей на дюйм экрана равно единице. В иных случая, размер frame buffer-а равен размеру окна за вычетом размера рамки, умноженному на к-во пикселей на дюйм экрана. Это, на самом деле, важный момент, который надо учитывать при рисовании в контекстном окне и более подробно мы к нему вернемся в следующей статье, а пока, остановимся на том, что уже знаем. Теперь, зная, как у нас описывается состояние событий, связанных с контекстным окном и его событиями, можно сделать служебные функции, через которые можно узнавать об этих событиях и о состоянии самого контекстного окна:

``` C++
namespace Frenchie
{
    namespace Application
    {
        // класс platfrom backend-а
        class ApplicationPlatformBackend final
        {
        public:

            ApplicationPlatformBackend() = delete;
            ApplicationPlatformBackend(const ApplicationPlatformBackend&) = delete;
            ApplicationPlatformBackend& operator=(const ApplicationPlatformBackend&) = delete;

            // открывает контекстное окно и запускает процесс инициализации графического API
            static bool awake();

            // кеширует события, связанные с контекстным окном, а также его свойства: размер окна, размер frame buffer-а, положение
            static void frame_start();

            // эта функция в основном нужна для сброса фиксации текстового ввода
            static void frame_update();

            // отображает в контекстном окне то, что нарисовала видеокарта и сбрасывает кеш событий
            static void frame_finish();

            // закрывает контекстное окно и запускает деинициализацию rendering backend-а
            static void quit();

            // проверяет, нужно ли закрыть контекстное окно
            static bool is_closed();

            // отдает команду на закрытие контекстного окна
            static void close();

            // служебные функции отслеживания переменных состояния событий, связанных с контекстным окном
            // и самого контекстного окна
            gs_vec2f get_window_cursor_position()
            {
                return m_Api->Input.MouseCursor.Position;
            }

            gs_vec2f get_window_cursor_dragdelta()
            {
                return m_Api->Input.MouseCursor.DragDelta;
            }

            gs_vec2f get_window_size()
            {
                return m_Api->Input.WindowSize;
            }

            gs_vec2f get_window_position()
            {
                return m_Api->Input.WindowPosition;
            }

            gs_vec2f get_window_framebuffer_size()
            {
                return m_Api->Input.FrameBufferSize;
            }

            gs_vec2f get_mouse_wheel_scroll_offset()
            {
                return m_Api->Input.MouseScrollOffset;
            }

            int key_clicks_count(const ApplicationPlatformBackendKey::Key& _Key)
            {
                return m_Api->Input.Keys[_Key].Clicks;
            }

            std::string input_text()
            {
                return m_Api->Input.Character.has_value() ?
                    Frenchie::Core::String::convert_utf32_to_utf8(std::u32string(1, m_Api->Input.Character.value())) :
                        std::string();
            }

            bool has_input_text()
            {
                return m_Api->Input.Character.has_value();
            }

            bool is_mouse_button_down(const ApplicationPlatformBackendMouseButton::Button& _Button)
            {
                return m_Api->Input.MouseButtons[_Button].Down;
            }

            bool is_mouse_button_hold(const ApplicationPlatformBackendMouseButton::Button& _Button)
            {
                return m_Api->Input.MouseButtons[_Button].Hold;
            }

            bool is_mouse_button_pressed(const ApplicationPlatformBackendMouseButton::Button& _Button)
            {
                return m_Api->Input.MouseButtons[_Button].Pressed;
            }

            bool is_mouse_button_released(const ApplicationPlatformBackendMouseButton::Button& _Button)
            {
                return m_Api->Input.MouseButtons[_Button].Released;
            }

            bool is_mouse_button_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button)
            {
                return m_Api->Input.MouseButtons[_Button].Clicked;
            }

            bool is_mouse_button_double_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button)
            {
                return m_Api->Input.MouseButtons[_Button].DoubleClicked;
            }

            int mouse_button_clicks_count(const ApplicationPlatformBackendMouseButton::Button& _Button)
            {
                return m_Api->Input.MouseButtons[_Button].Clicks;
            }

            bool is_key_down(const ApplicationPlatformBackendKey::Key& _Key)
            {
                return m_Api->Input.Keys[_Key].Down;
            }

            bool is_key_hold(const ApplicationPlatformBackendKey::Key& _Key)
            {
                return m_Api->Input.Keys[_Key].Hold;
            }

            bool is_key_pressed(const ApplicationPlatformBackendKey::Key& _Key)
            {
                return m_Api->Input.Keys[_Key].Pressed;
            }

            bool is_key_released(const ApplicationPlatformBackendKey::Key& _Key)
            {
                return m_Api->Input.Keys[_Key].Released;
            }

            bool is_key_clicked(const ApplicationPlatformBackendKey::Key& _Key)
            {
                return m_Api->Input.Keys[_Key].Clicked;
            }

            bool has_modifier(const ApplicationPlatformBackendKeyModifier::Modifier& _Key)
            {
                return m_Api->Input.Modifiers[_Key].Active;
            }

            // выдает состояние platform backend-а
            template <typename T = FrenchieApplicationPlatformApi>
            static std::shared_ptr<T> platform_api()
            {
                return std::dynamic_pointer_cast<T>(m_Api);
            }

        private:

            static std::shared_ptr<FrenchieApplicationPlatformApi> m_Api;

            // отслеживает события, связанные с контекстным окном, сохраняя их в кеш событий
            static void collect_input();

            // сбрасмывает кеш событий
            static void restore_input();
        };
    }
}
```

На данный момент мы узнали, каким образом описывается состояние контекстного окна, что есть в кеше его событий и как описываются переменные кеша. Теперь нужно разобраться в том, как по переменным кеша фиксируются события и как они сбрасываются. Сначала разберемся с логикой фиксации событий кнопок мыши и клавиатуры. Из написанного выше, в принципе, становится понятно (не с первого раза, но, становится), что для каждой кнопки мыши и клавиатуры нам нужно зафиксировать события нажатия, отпускания, зажатия, удержания и скликивания. Логику фиксации рассмотрим на примере кнопки мыши. Итак, чтобы зафиксировать упомянутые события для кнопки мыши нам нужно в цикле для каждой кнопки мыши зафиксировать момент времени нажатия на нее. Далее, если пользователь удерживает кнопку в зажатом состоянии, мы фиксируем, что она зажата. Если время зажатия превышает какой-то отрезок, называемый отрезком удержания, мы фиксируем событие удержания. Если же, пользователь отпустил кнопку, то мы фиксируем событие отпускания и если пользователь отпустил кнопку быстрее чем какой-то заданный промежуток времени, называемый временем скликивания, мы фиксируем событие скликивания. Наличие модификаторов мы фиксируем просто по факту зажатия кнопок-модификаторов. Вся описанная выше логика будет выглядеть так:

``` C++
void collect_input()
{
    // update application input input

    // отрезок времни, когда мы заканчимваем фиксировать к-во скликиваний кнопок мыши/клавиатуры
    const double KeyClicksCountResetTime = 200; // ms
    
    // отрезок времени фиксации удержания кнопки мыши/клавиатуры
    const double KeyHoldDetectionTime    = 100; // ms

    // отрезо времени фиксации скликивания кнопки мыши/клавиатуры
    const double KeyClickDetectionTime   = 500; // ms

    // логика фиксации событий кнопок мыши
    for (int mouseButton = ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonBegin;
             mouseButton < ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd;
             mouseButton++)
    {
        // здесь мы сбрасываем к-во кликов в случае, если пользователь отпустил кнопку мыши быстрее, чем время фиксации скликиваний
        if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].ReleaseTime,
            Frenchie::Core::Clock::tic()) > KeyClicksCountResetTime)
        {
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicks = 0;
        }

        // здесь мы фиксиурем момент времени нажатия на кнопку мыши и заодно сохраняем положение курсора в момент нажатия
        if(ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Pressed)
        {
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Down      = true;
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].PressTime = Frenchie::Core::Clock::tic();

            ApplicationPlatformBackend::platform_api()->Input.MouseCursor.MousePressPosition = ApplicationPlatformBackend::platform_api()->Input.MouseCursor.Position;
        }
        
        // здесь мы считаем время, в течение которого зажата кнопка мыши и заодно фиксируем отрезок перемещения курсора в сотоянии
        // зажатой кнопки мыши
        if(ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Down)
        {
            ApplicationPlatformBackend::platform_api()->Input.MouseCursor.DragDelta =
                ApplicationPlatformBackend::platform_api()->Input.MouseCursor.Position - ApplicationPlatformBackend::platform_api()->Input.MouseCursor.MousePressPosition;

            // если время зажатия кнопки мыши больше отрезка фиксации события удержания, мы фиксируем событие удержания кнопки мыши
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Hold =
                Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(
                    ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].PressTime,
                    Frenchie::Core::Clock::tic()) > KeyHoldDetectionTime;
        }

        // здесь, мы фиксируем событие отпускания кнопки мыши и сбрасываем события зажатия, удержания, а также
        // фиксируем момент времени отпускания кнопки мыши
        if(ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Released)
        {
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Down        = false;
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Hold        = false;
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].ReleaseTime = Frenchie::Core::Clock::tic();

            // если мы отпустили кнопку мыши за отрезок времени, меньший чем отрезок фиксации скликивания,
            // то мы фиксируем событие скликивания
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicked =
                Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(
                    ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].PressTime,
                    ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].ReleaseTime) < KeyClickDetectionTime;

            // здесь мы фиксируем двойно клик и заодно считаем количество самих кликов
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].DoubleClicked =
                ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicked && ++ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicks >= 2;
        }        
    }

    // логика фиксации событий кнопок клавиатуры, она идентична логике фиксации событий кнопок мыши
    for (int mouseButton = ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             mouseButton < ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_END;
             mouseButton++)
    {
        if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].ReleaseTime,
            Frenchie::Core::Clock::tic()) > KeyClicksCountResetTime)
        {
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Clicks = 0;
        }

        if(ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Pressed)
        {
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Down      = true;
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].PressTime = Frenchie::Core::Clock::tic();
        }
        
        if(ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Down)
        {
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Hold =
                Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(
                    ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].PressTime,
                    Frenchie::Core::Clock::tic()) > KeyHoldDetectionTime;
        }

        if(ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Released)
        {
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Down        = false;
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Hold        = false;
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].ReleaseTime = Frenchie::Core::Clock::tic();

            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Clicked =
                Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(
                    ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].PressTime,
                    ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].ReleaseTime) < KeyClickDetectionTime;

            ++ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Clicks;
        }
    }

    // ниже мы фиксируем модификаторы кнопок клавиатуры и мыши по факту зажатия кнопок, соответствующих этим модификаторам

    // Ctrl (Command on MacOS)
#ifdef FRENCHIE_APPLICATION_PLATFORM_IS_MACOS
    ApplicationPlatformBackend::platform_api()->Input.Modifiers[ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Ctrl].Active =
        ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftSuper].Down ||
            ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightSuper].Down;
#else
    ApplicationPlatformBackend::platform_api()->Input.Modifiers[ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Ctrl].Active =
        ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftCtrl].Down ||
            ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightCtrl].Down;
#endif

    // Alt (Option on MacOS)
    ApplicationPlatformBackend::platform_api()->Input.Modifiers[ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Alt].Active =
        ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftAlt].Down ||
            ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightAlt].Down;

    // Shift
    ApplicationPlatformBackend::platform_api()->Input.Modifiers[ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Shift].Active =
        ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftShift].Down ||
            ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightShift].Down;

    // ниже фиксируем символьный ввод символов '\n' и '\t' по факту нажатия соответствующих им кнопок

    // catch character '\n'
    if(ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_Enter].Pressed)
        ApplicationPlatformBackend::platform_api()->Input.Character = '\n';

    // catch character '\t'
    if(ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_Tab].Pressed)
        ApplicationPlatformBackend::platform_api()->Input.Character = '\t';
}
```

Сброс кеша событий контекстного окна выглядит сильно проще, чем фиксация, и думаю не требует пояснений:

``` C++
void restore_input()
{
    // сбрасываем состояние кнопок мыши
    for (int mouseButton = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             mouseButton < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             mouseButton++)
    {
        ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Released      = false;
        ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Pressed       = false;
        ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicked       = false;
        ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].DoubleClicked = false;
    }

    // сбрасываем состояние кнопок клавиатуры
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        ApplicationPlatformBackend::platform_api()->Input.Keys[key].Released = false;
        ApplicationPlatformBackend::platform_api()->Input.Keys[key].Pressed  = false;
        ApplicationPlatformBackend::platform_api()->Input.Keys[key].Clicked  = false;
    }

    // сбрасываем состояние модификаторов кнопок мыши и клавиатуры
    for (int key = ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_Begin;
             key < ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_End;
             key++)
    {
        ApplicationPlatformBackend::platform_api()->Input.Modifiers[key].Active = false;
    }

    // сбрасываем положение колесика мыши
    ApplicationPlatformBackend::platform_api()->Input.MouseScrollOffset = gs_vec2f(0.f, 0.f);

    // сбрасываем положение курсора
    ApplicationPlatformBackend::platform_api()->Input.MouseCursor.DragDelta = gs_vec2f(0.f, 0.f);

    // сбрасываем символьный ввод
    ApplicationPlatformBackend::platform_api()->Input.Character.reset();
}
```

На этом можно было бы выдохнуть, но, это еще не все. Дело в том, что, описывая логику фиксации событий клавиатуры и мыши я упустил то, как же мы фиксируем факт нажатия на кнопки, как фиксируется символьный ввод и как вооьще открывается контекстное окно ? Ну, если вы все еще не устали, то я продолжу. Итак, как уже мог догадаться опытный читатель, выше я описал лишь платформо-независимую логику, а вот логика, связананная с открытием контекстного окна, фиксацией нажатия, отпускания кнопок мыши и.т.д - это уже платформо-зависимый код, т.к упомянутые операции обычно выполняются функциями операционной системы. Тогда возникает вопрос: как же реализовать платформо-зависимую логику ? Здесь есть два пути. Первый путь - прокачать навыки системного программирования на всех популярных операционных системах и сделать все с нуля самостоятельно. Второй путь - использовать стороннюю библиотеку, которая уже дает какой-то слой абстракции над функциями операционной системы. Я в своем проекте на данный момент иду вторым путем. Так какие же сторонние библиотеки использовать ? Существует две очень популярные библиотеки для кроссплатформенного создания и открытия окон, запуска инициализации графического API и управления вводом. Первая - GLFW, вторая SDL3. Я в своем проекте реализовал функции platform backend-а с использованием обех упомянутых бибилиотек, но, в статье мы рассмотри только одну из них - GLFW, причем, мы рассотрим работу этой библиотеки в привязке к графическому API OpenGL. Почему GLFW и почему в привязке к OpenGL ? Как я уже писал ранее, запуск контексного окна тесно связан с запуском инициализации графического API и поэтому функция инициализации юужет привязана к конкретному графическому API. Плюс, GLFW компактней и проще чем SDL3. Итак, продолжим...

Чтобы открыть контекстное окно, запустить инициализацию графического API и активировать фиксацию событий нажания кнопки мыши, клавиатуры и.т.д с использованием библиотеки GLFW нам нужно создать контекстное окно, привязать созданное контекстное окно к графическому API и установить callback-и на фиксацию событий клавиатуры, мыши и перемещение и/или изменение размера контекстного окна:

``` C++
bool ApplicationPlatformBackend::awake()
{
    // если platform backend уже запущен, то не нужно делать это второй раз
    if(m_Api != nullptr && m_Api->Window != nullptr)
        return true;

    // инициализируем библиотеку GLFW
    if(glfwInit() == GLFW_FALSE)
        return false;

    // создаем состояние platform backend-а
    m_Api = std::make_shared<FrenchieApplicationPlatformApi>();

// на UNIX системах для OpenGL нужно выставить версию используемого OpenGL API,
// а также включить режим обратной совместимости. В виду того, что OpenGL устаревший,
// без указания версии и включения обратной совместимости, работать ничего не будет.
#ifdef FRENCHIE_APPLICATION_PLATFORM_IS_UNIX
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // выставляем параметры контекстного окна
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);   // окно не прозрачное
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE); // у окна есть рамка
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // окно может изменять размер
    glfwWindowHint(GLFW_ICONIFIED, GLFW_TRUE); // у окна можно поставить иконку
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);   // окно может получать фокус от операционной системы

    // создаем контекстное окно, если что-то пошло не так - выходим
    m_Api->Window = glfwCreateWindow(512, 256, "Application", nullptr, nullptr);

    if(m_Api->Window == nullptr)
    {
        glfwTerminate();
        return false;
    }

    // ставим callback-и на фиксацию событий, связанных с контекстным окном
    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(m_Api->Window));
    glfwSetWindowSizeCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_window_resize_callback);
    glfwSetFramebufferSizeCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_window_resize_callback);
    glfwSetWindowMaximizeCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_window_maximized_callback);
    glfwSetWindowFocusCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_window_focused_callback);
    glfwSetCursorEnterCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_cursor_enter_callback);
    glfwSetCursorPosCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_cursor_moved_callback);
    glfwSetMouseButtonCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_mouse_button_callback);
    glfwSetScrollCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_mouse_sroll_offset_changed_callback);
    glfwSetKeyCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_key_callback);
    glfwSetCharCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_character_input_callback);

    // запускаем процесс инициализации графического API через rendering backend
    if(!ApplicationRenderingBackend::awake((void*(*)(const char*))glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    // maximize context window
    glfwMaximizeWindow(reinterpret_cast<GLFWwindow*>(m_Api->Window));

    return true;
}
```

Далее, после запуска реализуем поведене окна внутри цикла работы приложения. Начало цикла с кешированием состояния контекстного окна будет выглядеть так:

``` C++
void ApplicationPlatformBackend::frame_start()
{
    // записываем размер frame buffer-а контекстного окна
    int x = 0;
    int y = 0;
    glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(m_Api->Window), &x, &y);
    m_Api->Input.FrameBufferSize = gs_vec2f(x, y);

    // записываем размер окна
    glfwGetWindowSize(reinterpret_cast<GLFWwindow*>(m_Api->Window), &x, &y);
    m_Api->Input.WindowSize = gs_vec2f(x, y);

    // записываем положение окна
    glfwGetWindowPos(reinterpret_cast<GLFWwindow*>(m_Api->Window), &x, &y);
    m_Api->Input.WindowPosition = gs_vec2f(x, y);

    // запускаем callback-и на фиксацию событий, связанных с контекстным окном
    glfwPollEvents();

    // устанавливаем интервал замены содержимого frame buffer-а контекстного окна
    glfwSwapInterval(1);

    // говорим видеокарте нарисовать графику, используя rendering backend
    ApplicationRenderingBackend::frame_start();

    // запускаем платформо-независимую логику кеширования событий контекстного окна
    ApplicationPlatformBackend::collect_input();
}

void ApplicationPlatformBackend::frame_update()
{
    // здесь ничего не происходит, т.к GLFW не требует ручного сброса текстового воода и/или фиксации событий
}
```
В коде выше мы установили интервал обновления frame buffer-а контекстного окна. Что же это за интервал ? Дело в том, что прежде чем нарисовать что-то новое, контекстное окно очищает свой frame buffer и потом уже рисует в него то, что мы забрали с видеокарты. Интервал обновления frame buffer-а определяет, раз в какое время мы будем очищать содержимое frame buffer-а и выставлять новое его содержимое. Но, как заметил бы опытный читатель, если бы все работало так, как описано, то окно мерцало бы, т.е мы бы видели его очистку. Да, действительно это было бы так, если бы у окна был только один frame buffer. Но, на всех современных операционных системах контекстные окна имеют два frame buffer-а и пока на показывается текущий frame buffer, рисуется следующий и в момент, когда надо обновить содержимое контекстного окна мы просто меняем текущий frame buffer с тем, что нарисовался. Вот и все, и делается это с заданной периодичностью, которую мы установивли в коде выше. Итак, что же дальеш ? В конце кадра работы приложения нам нужно попросить контекстное окно нарисовать то, что нарисовала нам видеокарта (т.е попросить окно поменять текущий frame buffer на тот, что нарисовался) и сбросить кеш событий:

``` C++
void ApplicationPlatformBackend::frame_finish()
{
    // Просим видеокарту отдать нам то, что она нарисовала с использованием rendering backend-а.
    ApplicationRenderingBackend::frame_finish();

    // подменяем то, что уже есть во frame buffer-е контекстного окна на то, что нарисовала видеокарта
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(m_Api->Window));

    // запускаем платформо-независимую логику сброса событий, связанных с контекстным окном
    ApplicationPlatformBackend::restore_input();
}
```

Когда приложение закрывается, нам нужно деинициализировать графический API и закрыть контекстное окно. Делается это вот так:

``` C++
void ApplicationPlatformBackend::quit()
{
    // terminate rendering API
    ApplicationRenderingBackend::quit();

    // terminate self
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_Api->Window));
    glfwTerminate();
    m_Api->Window = nullptr;
}
```

Ну, на этом точно можно было бы выдохнуть, но, я же не рассказал про callback-и, через которые мы фиксируем события. Вот как раз эти callback-и:

``` C++
namespace Frenchie
{
    namespace Application
    {
        class FrenchieApplicationGLFWInputHandler
        {
        public:

            // этой функцией мы конвертируем названия кнопок мыши библиотеки GLFW в наши названия
            static ApplicationPlatformBackendMouseButton::Button glfw_mouse_button_to_application_mouse_button(int _MouseButton);

            // У библиотеки GLFW свое определение булевым переменным, поэтому нужно их конвертировать в стандарный bool языка C++
            static bool glfw_boolean_to_application_boolean(int _Boolean);

            // этой функцией мы конвертируем названия клавиш клавиатуры библиотеки GLFW в наши названия
            static ApplicationPlatformBackendKey::Key glfw_key_to_application_key(int _KeyKode, int = -1);

            // этот callback вызывается, когда контекстное окно изменяет свой размер
            static void glfw_on_window_resize_callback(GLFWwindow* _Window, int _Width, int _Height);

            // этот callback вызывается, когда контекстное окно растягивается во весь экран
            static void glfw_on_window_maximized_callback(GLFWwindow* _Window, int _Maximized);

            // этот callback вызывается, когда контекстное окно получает фокус от операционной системы
            static void glfw_on_window_focused_callback(GLFWwindow* _Window, int _Focused);

            // этот callback вызывается каждый раз, когда курсор двигается
            static void glfw_on_cursor_moved_callback(GLFWwindow* _Window, double _X, double _Y);

             // этот callback вызывается каждый раз, когда курсор заходит в контекстное окно
            static void glfw_on_cursor_enter_callback(GLFWwindow* _Window, int _Entered);

            // этот callback вызывается каждый раз, когда нажимается или отпускается кнопка мыши,
            // да, тут как раз мы и фиксируем факт нажатия и отпускания кнопки мыши
            static void glfw_on_mouse_button_callback(GLFWwindow* _Window, int button, int action, int mods);
        
            // этот callback вызывается каждый раз, когда нажимается или отпускается кнопка клавиатуры,
            // да, тут как раз мы и фиксируем факт нажатия и отпускания кнопки клавиатуры
            static void glfw_on_key_callback(GLFWwindow* _Window, int _Keycode, int _Scancode, int _Action, int _Mods);
        
            // этот callback вызывается каждый раз, когда мы вводим текст в контекстнои окне
            static void glfw_on_character_input_callback(GLFWwindow* _Window, unsigned int _Character);

            // этот callback вызывается каждый раз, когда колесико мыши поворачивается
            static void glfw_on_mouse_sroll_offset_changed_callback(GLFWwindow* _Window, double _dX, double _dY);
        };
    }
}
```

Все описанные выше callback-и вызываются в моменты, когда нажимается кнопка мыши или клавиатуры и/или контекстное окно меняет свой размер/положение, фокус, а также когда в нем меняется положение курсора. На этом, предлагаю остановиться, дабы статья не раздулась до вселенских масштабов. Конкретную реализацию указанных выше callback-ов и ряда других служебных фукнций предлагаю посмотреть в репозитории самой библиотеки. Надеюсь, никто не устал и было интересно. Спасибо за внимание.