#include <stdint.h> // تمت إضافة هذه المكتبة لاستخدام uint32_t
#include <stddef.h> // تمت إضافة هذه المكتبة لاستخدام NULL

#define VIDEO_MEMORY 0xB8000
#define MAX_TASKS 10
#define STACK_SIZE 1024
#define MEMORY_SIZE 1024 * 1024
#define MAX_FILES 10
#define FILE_NAME_LENGTH 20
#define MAX_PROCESSES 10
#define KEYBOARD_DATA_PORT 0x60
#define MAX_COMMANDS 10
#define COMMAND_LENGTH 50
#define MAX_NETWORK_PACKETS 10
#define PACKET_SIZE 100
#define MAX_DEVICES 10
#define DEVICE_NAME_LENGTH 20
#define MAX_USERS 10
#define USER_NAME_LENGTH 20

struct task {
    int id;
    void (*function)();
    uint32_t esp;
    uint32_t stack[STACK_SIZE];
};

struct file {
    char name[FILE_NAME_LENGTH];
    int size;
    char* data;
};

struct memory_block {
    int size;
    int free;
    struct memory_block* next;
};

struct process {
    int pid;
    char name[20];
    int status;
};

struct command {
    char name[COMMAND_LENGTH];
    void (*function)();
};

struct network_packet {
    char data[PACKET_SIZE];
    int size;
};

struct device {
    char name[DEVICE_NAME_LENGTH];
    int type;
    int status;
};

struct user {
    char name[USER_NAME_LENGTH];
    int uid;
    int gid;
};

struct task tasks[MAX_TASKS];
int current_task = 0;
char memory_pool[MEMORY_SIZE];
struct memory_block* free_list;
struct file files[MAX_FILES];
int file_count = 0;
struct process processes[MAX_PROCESSES];
int process_count = 0;
struct command commands[MAX_COMMANDS];
int command_count = 0;
struct network_packet network_packets[MAX_NETWORK_PACKETS];
int packet_count = 0;
struct device devices[MAX_DEVICES];
int device_count = 0;
struct user users[MAX_USERS];
int user_count = 0;
static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "d"(port));
    return result;
}

void clear_screen() {
    char* video_memory = (char*) VIDEO_MEMORY;
    for (int i = 0; i < 80 * 25 * 2; i++) {
        video_memory[i] = 0;
    }
}

void print_string(const char* str) {
    char* video_memory = (char*) VIDEO_MEMORY;
    while (*str) {
        *video_memory++ = *str++;
        *video_memory++ = 0x07;
    }
}

void print_number(int num) {
    char* video_memory = (char*) VIDEO_MEMORY + 160;
    if (num == 0) {
        *video_memory++ = '0';
        *video_memory++ = 0x07;
        return;
    }
    char buffer[20];
    int i = 0;
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    for (int j = i - 1; j >= 0; j--) {
        *video_memory++ = buffer[j];
        *video_memory++ = 0x07;
    }
}

void task1() {
    while (1) {
        print_string("Task 1");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

void task2() {
    while (1) {
        print_string("Task 2");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

void schedule() {
    tasks[current_task].function();
    current_task = (current_task + 1) % 2;
}

void initialize_memory() {
    free_list = (struct memory_block*) memory_pool;
    free_list->size = MEMORY_SIZE - sizeof(struct memory_block);
    free_list->free = 1;
    free_list->next = NULL;
}

void* allocate_memory(int size) {
    struct memory_block* current = free_list;
    while (current) {
        if (current->free && current->size >= size) {
            current->free = 0;
            return (void*) (current + 1);
        }
        current = current->next;
    }
    return NULL;
}

void free_memory(void* ptr) {
    struct memory_block* block = (struct memory_block*) ptr - 1;
    block->free = 1;
}

void create_file(const char* name, int size) {
    if (file_count >= MAX_FILES) return;
    struct file* new_file = &files[file_count++];
    for (int i = 0; i < FILE_NAME_LENGTH; i++) {
        new_file->name[i] = name[i];
        if (name[i] == '\0') break;
    }
    new_file->size = size;
    new_file->data = allocate_memory(size);
}

void list_files() {
    for (int i = 0; i < file_count; i++) {
        print_string(files[i].name);
        print_string(" ");
        print_number(files[i].size);
        print_string(" bytes\n");
    }
}

void create_process(const char* name) {
    if (process_count >= MAX_PROCESSES) return;
    struct process* new_process = &processes[process_count++];
    new_process->pid = process_count;
    for (int i = 0; i < 20; i++) {
        new_process->name[i] = name[i];
        if (name[i] == '\0') break;
    }
    new_process->status = 1;
}

void list_processes() {
    for (int i = 0; i < process_count; i++) {
        print_string("PID: ");
        print_number(processes[i].pid);
        print_string(" Name: ");
        print_string(processes[i].name);
        print_string(" Status: ");
        print_number(processes[i].status);
        print_string("\n");
    }
}

char read_keyboard() {
    char key = 0;
    while ((key = inb(KEYBOARD_DATA_PORT)) == 0);
    return key;
}

void add_command(const char* name, void (*function)()) {
    if (command_count >= MAX_COMMANDS) return;
    struct command* new_command = &commands[command_count++];
    for (int i = 0; i < COMMAND_LENGTH; i++) {
        new_command->name[i] = name[i];
        if (name[i] == '\0') break;
    }
    new_command->function = function;
}

void execute_command(const char* name) {
    for (int i = 0; i < command_count; i++) {
        int match = 1;
        for (int j = 0; j < COMMAND_LENGTH; j++) {
            if (commands[i].name[j] != name[j]) {
                match = 0;
                break;
            }
            if (name[j] == '\0') break;
        }
        if (match) {
            commands[i].function();
            return;
        }
    }
    print_string("Command not found: ");
    print_string(name);
    print_string("\n");
}

void command_help() {
    print_string("Available commands:\n");
    for (int i = 0; i < command_count; i++) {
        print_string(commands[i].name);
        print_string("\n");
    }
}

void command_clear() {
    clear_screen();
}

void send_network_packet(const char* data, int size) {
    if (packet_count >= MAX_NETWORK_PACKETS) return;
    struct network_packet* new_packet = &network_packets[packet_count++];
    for (int i = 0; i < size; i++) {
        new_packet->data[i] = data[i];
    }
    new_packet->size = size;
}

void receive_network_packet(char* buffer, int* size) {
    if (packet_count == 0) return;
    struct network_packet* packet = &network_packets[--packet_count];
    for (int i = 0; i < packet->size; i++) {
        buffer[i] = packet->data[i];
    }
    *size = packet->size;
}

void add_device(const char* name, int type) {
    if (device_count >= MAX_DEVICES) return;
    struct device* new_device = &devices[device_count++];
    for (int i = 0; i < DEVICE_NAME_LENGTH; i++) {
        new_device->name[i] = name[i];
        if (name[i] == '\0') break;
    }
    new_device->type = type;
    new_device->status = 1;
}

void list_devices() {
    for (int i = 0; i < device_count; i++) {
        print_string("Device: ");
        print_string(devices[i].name);
        print_string(" Type: ");
        print_number(devices[i].type);
        print_string(" Status: ");
        print_number(devices[i].status);
        print_string("\n");
    }
}

void add_user(const char* name, int uid, int gid) {
    if (user_count >= MAX_USERS) return;
    struct user* new_user = &users[user_count++];
    for (int i = 0; i < USER_NAME_LENGTH; i++) {
        new_user->name[i] = name[i];
        if (name[i] == '\0') break;
    }
    new_user->uid = uid;
    new_user->gid = gid;
}

void list_users() {
    for (int i = 0; i < user_count; i++) {
        print_string("User: ");
        print_string(users[i].name);
        print_string(" UID: ");
        print_number(users[i].uid);
        print_string(" GID: ");
        print_number(users[i].gid);
        print_string("\n");
    }
}

void kernel_main() {
    clear_screen();
    initialize_memory();
    tasks[0].id = 1;
    tasks[0].function = task1;
    tasks[1].id = 2;
    tasks[1].function = task2;

    create_file("file1.txt", 100);
    create_file("file2.txt", 200);
    list_files();

    create_process("init");
    create_process("bash");
    list_processes();

    add_command("help", command_help);
    add_command("clear", command_clear);

    send_network_packet("Hello, World!", 13);
    char buffer[100];
    int size;
    receive_network_packet(buffer, &size);
    print_string("Received packet: ");
    print_string(buffer);
    print_string("\n");

    add_device("Keyboard", 1);
    add_device("Mouse", 2);
    list_devices();

    add_user("root", 0, 0);
    add_user("user", 1000, 1000);
    list_users();

    while (1) {
        schedule();
        char key = read_keyboard();
        print_string("Key pressed: ");
        print_number(key);
        print_string("\n");
    }
}
