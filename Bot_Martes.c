// Bibliotecas utilizadas
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Variables de inicializacion de Chrome
#define CHROME_PATH "C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe"
#define URL "https://amazon.com"

// Posición y tamaño de ventana
#define WIN_X 0
#define WIN_Y 0
#define WIN_WIDTH 1240
#define WIN_HEIGHT 720

// Coordenadas del botón "Start Scraping" dentro de la ventana Chrome
#define SCRAPE_BTN_X 980
#define SCRAPE_BTN_Y 160

// Tiempos de espera en milisegundos
#define WAIT_TIME 5000
#define CLICK_DURATION 1000
#define KEY_DURATION 1000

// Número máximo de intentos para cada acción
#define MAX_RETRIES 3

// Nombre del archivo de log
#define LOG_FILENAME "log.txt"

// Archivo de log global
FILE* g_log_file = NULL;

// Estructura para almacenar información de ventana
typedef struct {
    char className[256];
    char title[256];
    HWND hwnd;
} WindowInfo;

// Función para obtener la ruta del ejecutable
void get_executable_path(char* buffer, int buffer_size) {
    GetModuleFileName(NULL, buffer, buffer_size);
    
    // Encontrar la última barra para obtener solo el directorio
    char* last_slash = strrchr(buffer, '\\');
    if (last_slash != NULL) {
        *(last_slash + 1) = '\0'; // Truncar después de la última barra
    }
}

// Función para inicializar el archivo de log
void init_log() {
    char executable_path[MAX_PATH];
    char log_path[MAX_PATH];
    
    get_executable_path(executable_path, MAX_PATH);
    sprintf(log_path, "%s%s", executable_path, LOG_FILENAME);
    
    // Abrir el archivo en modo escritura (sobrescribir si existe)
    g_log_file = fopen(log_path, "w");
    if (g_log_file == NULL) {
        // Si no se puede abrir el archivo, intentamos en el directorio actual
        g_log_file = fopen(LOG_FILENAME, "w");
    }
    
    // Si aún no se puede crear, no hay mucho que podamos hacer
    if (g_log_file == NULL) {
        return;
    }
    
    // Escribir encabezado con fecha y hora
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    
    char timestamp[26];
    strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(g_log_file, "=== AutoScrap1 Log iniciado: %s ===\n\n", timestamp);
    fflush(g_log_file);
}

// Función para escribir en el log
void log_message(const char* format, ...) {
    if (g_log_file == NULL) return;
    
    // Obtener timestamp
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    
    char timestamp[26];
    strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    // Escribir timestamp
    fprintf(g_log_file, "[%s] ", timestamp);
    
    // Escribir mensaje con argumentos variables
    va_list args;
    va_start(args, format);
    vfprintf(g_log_file, format, args);
    va_end(args);
    
    // Asegurar que se escriba al archivo inmediatamente
    fprintf(g_log_file, "\n");
    fflush(g_log_file);
}

// Función para cerrar el log
void close_log() {
    if (g_log_file != NULL) {
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        
        char timestamp[26];
        strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        
        fprintf(g_log_file, "\n=== AutoScrap1 Log finalizado: %s ===\n", timestamp);
        fclose(g_log_file);
        g_log_file = NULL;
    }
}

// Función para verificar si una ventana específica está activa
BOOL is_window_active(const char* className, const char* windowTitle) {
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL) return FALSE;
    
    char currentClassName[256] = {0};
    char currentTitle[256] = {0};
    
    GetClassName(hwnd, currentClassName, sizeof(currentClassName));
    GetWindowText(hwnd, currentTitle, sizeof(currentTitle));
    
    // Verificar clase si se proporciona
    if (className != NULL && strcmp(className, "") != 0) {
        if (strcmp(currentClassName, className) != 0) return FALSE;
    }
    
    // Verificar título si se proporciona
    if (windowTitle != NULL && strcmp(windowTitle, "") != 0) {
        // Buscar subcadena en el título de la ventana
        if (strstr(currentTitle, windowTitle) == NULL) return FALSE;
    }
    
    return TRUE;
}

// Función para activar una ventana específica
BOOL activate_window(const char* className, const char* windowTitle) {
    HWND hwnd = NULL;
    
    // Si ambos son NULL, usamos la ventana de primer plano actual
    if ((className == NULL || strcmp(className, "") == 0) && 
        (windowTitle == NULL || strcmp(windowTitle, "") == 0)) {
        return TRUE;
    }
    
    // Buscar por título exacto primero
    if (windowTitle != NULL && strcmp(windowTitle, "") != 0) {
        hwnd = FindWindow(className, windowTitle);
    }
    
    // Si no se encontró, buscar por título parcial
    if (hwnd == NULL && windowTitle != NULL && strcmp(windowTitle, "") != 0) {
        HWND hwndTemp = GetTopWindow(NULL);
        while (hwndTemp != NULL) {
            char title[256] = {0};
            char classNameBuf[256] = {0};
            
            GetWindowText(hwndTemp, title, sizeof(title));
            GetClassName(hwndTemp, classNameBuf, sizeof(classNameBuf));
            
            // Verificar si el título contiene la cadena buscada
            if (strstr(title, windowTitle) != NULL) {
                // Si se especificó className, verificar también
                if (className == NULL || strcmp(className, "") == 0 || strcmp(classNameBuf, className) == 0) {
                    hwnd = hwndTemp;
                    break;
                }
            }
            
            hwndTemp = GetNextWindow(hwndTemp, GW_HWNDNEXT);
        }
    }
    
    // Si no se encontró por título, buscar solo por clase
    if (hwnd == NULL && className != NULL && strcmp(className, "") != 0) {
        hwnd = FindWindow(className, NULL);
    }
    
    if (hwnd != NULL) {
        // Verificar si la ventana está minimizada
        if (IsIconic(hwnd)) {
            ShowWindow(hwnd, SW_RESTORE);
        }
        
        // Colocar la ventana en primer plano
        SetForegroundWindow(hwnd);
        Sleep(WAIT_TIME); // Esperar a que la ventana tome el foco
        
        return is_window_active(className, windowTitle);
    }
    
    return FALSE;
}

// Definiciones de teclas
void simulate_key(WORD key, DWORD flag) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = flag;
    SendInput(1, &input, sizeof(INPUT));
}

// Función para presionar una tecla con verificación
BOOL press_key_with_verification(WORD key) {
    // Asegurar que tenemos el foco correcto antes de enviar teclas
    HWND hwndFocus = GetForegroundWindow();
    if (hwndFocus == NULL) return FALSE;
    
    log_message("Presionando tecla: 0x%X", key);
    
    simulate_key(key, 0);
    Sleep(KEY_DURATION); // Mantener tecla presionada por 1 segundo
    simulate_key(key, KEYEVENTF_KEYUP);
    Sleep(WAIT_TIME - KEY_DURATION); // Completar el tiempo de espera total
    
    return TRUE;
}

// Función para presionar una combinación de teclas
BOOL press_key_combination(WORD* keys, int keyCount) {
    // Asegurar que tenemos el foco correcto
    HWND hwndFocus = GetForegroundWindow();
    if (hwndFocus == NULL) return FALSE;
    
    log_message("Presionando combinación de %d teclas", keyCount);
    
    // Presionar todas las teclas
    for (int i = 0; i < keyCount; i++) {
        simulate_key(keys[i], 0);
        Sleep(100); // Pequeña pausa entre teclas
    }
    
    Sleep(KEY_DURATION); // Mantener combinación presionada
    
    // Soltar todas las teclas en orden inverso
    for (int i = keyCount - 1; i >= 0; i--) {
        simulate_key(keys[i], KEYEVENTF_KEYUP);
        Sleep(100); // Pequeña pausa entre teclas
    }
    
    Sleep(WAIT_TIME - KEY_DURATION); // Completar tiempo de espera
    
    return TRUE;
}

// Función para hacer click con verificación
BOOL click_with_verification(int x, int y) {
    // Guardar posición original del cursor
    POINT originalPos;
    GetCursorPos(&originalPos);
    
    log_message("Click en posición: (%d, %d)", x, y);
    
    // Mover a la nueva posición
    SetCursorPos(x, y);
    Sleep(WAIT_TIME / 2); // Esperar a que el cursor llegue
    
    // Realizar click con duración de 1 segundo
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    Sleep(CLICK_DURATION);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    
    // Completar tiempo de espera
    Sleep(WAIT_TIME - CLICK_DURATION);
    
    // Verificar que el cursor realmente se movió
    POINT currentPos;
    GetCursorPos(&currentPos);
    
    return (currentPos.x == x && currentPos.y == y);
}

// Función para presionar Ctrl + Shift + P con verificación
BOOL ctrl_shift_p() {
    WORD keys[] = {VK_CONTROL, VK_SHIFT, 0x50}; // Ctrl, Shift, P
    return press_key_combination(keys, 3);
}

// Función para reintentar una acción
BOOL retry_action(BOOL (*action)(), const char* actionName) {
    for (int attempt = 1; attempt <= MAX_RETRIES; attempt++) {
        log_message("Intentando %s (intento %d de %d)...", actionName, attempt, MAX_RETRIES);
        if (action()) {
            log_message("%s completado exitosamente.", actionName);
            return TRUE;
        }
        log_message("Fallo en %s. Reintentando...", actionName);
        Sleep(WAIT_TIME); // Esperar antes de reintentar
    }
    log_message("ERROR: %s falló después de %d intentos.", actionName, MAX_RETRIES);
    return FALSE;
}

// Función para cerrar ventanas de Chrome
BOOL CALLBACK CloseChromeWindows(HWND hwnd, LPARAM lParam) {
    char className[256];
    GetClassName(hwnd, className, sizeof(className));
    if (strcmp(className, "Chrome_WidgetWin_1") == 0) {
        SendMessage(hwnd, WM_CLOSE, 0, 0);
        Sleep(WAIT_TIME);
    }
    return TRUE;
}

// Función para verificar si VMware está en ejecución
BOOL is_vmware_running() {
    HWND hwnd = FindWindow("VMwareUnityHostWndClass", NULL);
    if (hwnd != NULL) {
        return TRUE;
    }
    
    // También verificar la ventana principal de VMware
    hwnd = FindWindow(NULL, "VMware Workstation");
    return (hwnd != NULL);
}

// Función para obtener la última ruta de máquina virtual
char* get_last_vm_path() {
    static char vmx_path[1024] = {0};
    char prefs_path[MAX_PATH];
    sprintf(prefs_path, "%s\\AppData\\Roaming\\VMware\\preferences.ini", getenv("USERPROFILE"));

    FILE* prefs = fopen(prefs_path, "r");
    if (!prefs) return NULL;

    char line[1024];
    while (fgets(line, sizeof(line), prefs)) {
        if (strncmp(line, "pref.mruVM0 =", 13) == 0) {
            char* start = strchr(line, '=');
            if (start) {
                start += 2; // skip "= "
                char* end = strchr(start, '\n');
                if (end) *end = 0;
                strncpy(vmx_path, start, sizeof(vmx_path)-1);
                fclose(prefs);
                return vmx_path;
            }
        }
    }
    fclose(prefs);
    return NULL;
}

// Funciones para acciones específicas que se pueden reintentar
BOOL launch_chrome() {
    char command[1024];
    sprintf(command, "\"%s\" --new-window \"%s\" --window-size=1200,720 --window-position=0,0", CHROME_PATH, URL);
    WinExec(command, SW_SHOW);
    Sleep(WAIT_TIME * 2); // Esperar más tiempo para que Chrome inicie
    
    // Verificar que Chrome está abierto
    return activate_window("Chrome_WidgetWin_1", "Amazon");
}

BOOL maximize_window() {
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL) return FALSE;
    
    WORD keys[] = {VK_LWIN, VK_UP}; // Win + Up
    BOOL result = press_key_combination(keys, 2);
    
    // Verificar que la ventana se maximizó
    WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
    GetWindowPlacement(hwnd, &wp);
    
    return (wp.showCmd == SW_SHOWMAXIMIZED);
}

BOOL open_devtools() {
    WORD keys[] = {VK_CONTROL, VK_SHIFT, 0x4A}; // Ctrl + Shift + J
    BOOL result = press_key_combination(keys, 3);
    
    // Esperar a que se abra DevTools
    Sleep(WAIT_TIME);
    
    // Verificar que estamos en DevTools
    return is_window_active("Chrome_WidgetWin_1", "DevTools");
}

BOOL activate_vmware() {
    return activate_window("VMwareUnityHostWndClass", "VMware");
}

// Función principal -- Flujo del programa
int main() {
    // Inicializar el archivo de log
    init_log();
    
    log_message("Iniciando proceso de scraping...");

    // Cerrar todas las ventanas de Chrome existentes
    log_message("Cerrando ventanas de Chrome existentes...");
    for (int i = 0; i < 5; ++i) {
        EnumWindows(CloseChromeWindows, 0);
        Sleep(WAIT_TIME);
    }
    
    // Lanzar Chrome con reintento
    if (!retry_action(launch_chrome, "Lanzar Chrome")) {
        log_message("Error crítico: No se pudo iniciar Chrome después de varios intentos");
        close_log();
        return 1;
    }
    
    // Maximizar ventana con reintento
    if (!retry_action(maximize_window, "Maximizar ventana")) {
        log_message("Error crítico: No se pudo maximizar la ventana");
        close_log();
        return 1;
    }
    
    // Obtener ventana de Chrome y forzar tamaño
    HWND hwnd = FindWindow("Chrome_WidgetWin_1", NULL);
    if (hwnd == NULL) {
        log_message("Error crítico: No se encontró la ventana de Chrome");
        close_log();
        return 1;
    }
    
    SetWindowPos(hwnd, HWND_TOP, WIN_X, WIN_Y, WIN_WIDTH, WIN_HEIGHT, SWP_SHOWWINDOW);
    SetForegroundWindow(hwnd);
    Sleep(WAIT_TIME);
    
    // Abrir DevTools con reintento
    if (!retry_action(open_devtools, "Abrir DevTools")) {
        log_message("Error crítico: No se pudo abrir DevTools");
        close_log();
        return 1;
    }
    
    // Maximizar DevTools
    if (!retry_action(maximize_window, "Maximizar DevTools")) {
        log_message("Error crítico: No se pudo maximizar DevTools");
        close_log();
        return 1;
    }
    
    log_message("Navegando a WebScraper...");
    
    // Secuencia de clicks para configurar el scraping
    // Cada click se hace con verificación y tiempo de espera
    
    // Click en el botón inspeccionar
    if (!activate_window("Chrome_WidgetWin_1", "DevTools")) {
        log_message("Error crítico: No se pudo activar la ventana de DevTools");
        close_log();
        return 1;
    }
    click_with_verification(170, 523);
    Sleep(WAIT_TIME);
    
    // Click en la pestaña de web scraper
    if (!activate_window("Chrome_WidgetWin_1", "DevTools")) {
        log_message("Error: No se pudo activar la ventana para la pestaña Web Scraper");
        close_log();
        return 1;
    }
    click_with_verification(916, 40);
    Sleep(WAIT_TIME);
    
    // Click en el primer botón de sitemap
    if (!activate_window("Chrome_WidgetWin_1", "DevTools")) {
        log_message("Error: No se pudo activar la ventana para el botón de sitemap");
        close_log();
        return 1;
    }
    click_with_verification(50, 58);
    Sleep(WAIT_TIME);
    
    // Click en el primer sitemap
    if (!activate_window("Chrome_WidgetWin_1", "DevTools")) {
        log_message("Error: No se pudo activar la ventana para seleccionar el sitemap");
        close_log();
        return 1;
    }
    click_with_verification(300, 190);
    Sleep(WAIT_TIME);
    
    // Click en el menú scrap
    if (!activate_window("Chrome_WidgetWin_1", "DevTools")) {
        log_message("Error: No se pudo activar la ventana para el menú scrap");
        close_log();
        return 1;
    }
    click_with_verification(152, 67);
    Sleep(WAIT_TIME);
    
    // Click en start scrap
    if (!activate_window("Chrome_WidgetWin_1", "DevTools")) {
        log_message("Error: No se pudo activar la ventana para iniciar scrap");
        close_log();
        return 1;
    }
    click_with_verification(173, 179);
    Sleep(WAIT_TIME);
    
    // Presionar Tab
    if (!activate_window("Chrome_WidgetWin_1", "DevTools")) {
        log_message("Error: No se pudo activar la ventana para presionar Tab");
        close_log();
        return 1;
    }
    press_key_with_verification(VK_TAB);
    
    // Ingresar "6000"
    if (!activate_window("Chrome_WidgetWin_1", "DevTools")) {
        log_message("Error: No se pudo activar la ventana para ingresar 6000");
        close_log();
        return 1;
    }
    press_key_with_verification(0x36); // 6
    press_key_with_verification(0x30); // 0
    press_key_with_verification(0x30); // 0
    press_key_with_verification(0x30); // 0
    
    // Presionar Tab
    if (!activate_window("Chrome_WidgetWin_1", "DevTools")) {
        log_message("Error: No se pudo activar la ventana para presionar Tab");
        close_log();
        return 1;
    }
    press_key_with_verification(VK_TAB);
    
    // Ingresar "7000"
    if (!activate_window("Chrome_WidgetWin_1", "DevTools")) {
        log_message("Error: No se pudo activar la ventana para ingresar 7000");
        close_log();
        return 1;
    }
    press_key_with_verification(0x37); // 7
    press_key_with_verification(0x30); // 0
    press_key_with_verification(0x30); // 0
    press_key_with_verification(0x30); // 0
    
    // Presionar Tab
    if (!activate_window("Chrome_WidgetWin_1", "DevTools")) {
        log_message("Error: No se pudo activar la ventana para presionar Tab");
        close_log();
        return 1;
    }
    press_key_with_verification(VK_TAB);
    
    // Click en start scrap final
    if (!activate_window("Chrome_WidgetWin_1", "DevTools")) {
        log_message("Error: No se pudo activar la ventana para el start scrap final");
        close_log();
        return 1;
    }
    click_with_verification(215, 150);
    
    // Esperar a que comience el scraping
    log_message("Scraping iniciado. Esperando 20 segundos...");
    Sleep(20000);
    

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

    log_message("Iniciando VMware Workstation...");

    //Validar que no exista una instancia activa del programa
    // En caso de haberla, no abrir una nueva y hacer tab y enter

    if (is_vmware_running()) {
        log_message("VMware ya está en ejecución, no se abrirá una nueva instancia.");
        // Hacer Tab y Enter
        press_key_with_verification(VK_TAB);
        press_key_with_verification(VK_RETURN);
    } else {
        // Abrir VMware Workstation y ejecutar la última VM
        const char* vmware_path = "C:\\Program Files (x86)\\VMware\\VMware Workstation\\vmware.exe";
        char* last_vmx = get_last_vm_path();
        char vmware_cmd[2048];
        
        if (last_vmx && strlen(last_vmx) > 0) {
            sprintf(vmware_cmd, "\"%s\" -x \"%s\"", vmware_path, last_vmx);
            log_message("Ejecutando VM: %s", last_vmx);
        } else {
            sprintf(vmware_cmd, "\"%s\"", vmware_path);
            log_message("No se encontró VM reciente, abriendo VMware sin VM");
        }
        
        WinExec(vmware_cmd, SW_SHOW);
        
        // Esperar a que se abra VMware y verificar que está activo
        log_message("Esperando a que se inicie VMware...");
        Sleep(WAIT_TIME * 2);
        
        // Intentar activar la ventana de VMware
        if (!retry_action(activate_vmware, "Activar VMware")) {
            log_message("Error: No se pudo activar VMware, pero continuando...");
        }
        
        // Esperar a que la máquina virtual se inicie
        log_message("Esperando a que la máquina virtual se inicie (30 segundos)...");
        Sleep(30000);

        // Interactuar con la VM
        if (!activate_window("VMwareUnityHostWndClass", "VMware")) {
            log_message("Error: No se pudo activar la ventana de VMware para interacción");
        }
    }
    // Esperar a que la VM termine de iniciar
    log_message("Esperando a que la VM termine de iniciar (1 minuto)...");
    Sleep(60000);
    

    // Cerrar el archivo de log
    press_key_with_verification(VK_RETURN);
    log_message("Enviando secuencia de teclas a la VM: Tab");
    press_key_with_verification(VK_TAB);
    log_message("Enviando secuencia de teclas a la VM: Space");
    press_key_with_verification(VK_SPACE);
    
    // Esperar a que la VM termine de iniciar
    log_message("Esperando a que la VM termine de iniciar (1 minuto)...");
    Sleep(60000);
*/

    // Cerrar el archivo de log
    close_log();
    
    return 0;

}