#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <unistd.h>

#define MAX_LINE_LEN 1024 /* Maximum characters per line */

bool show_size = false;    /* Whether to show size of struct */
bool swap_pointer = false; /* Whether to use pointer in swap function */
bool time_record = false;  /* Whether to record the time */

/*
* Overview: Structure for specifying date.
* @type: {int} y - Year.
* @type: {int} m - Month.
* @type: {int} d - Day.
*/
struct date
{
    int y;
    int m;
    int d;
};

/*
* Overview: Structure for specifying user profiles.
* @type: {int} id - ID.
* @type: {char} name - Name.
* @type: {struct date} birthday - Birthday.
* @type: {char} address - Address.
* @type: {char} note - Others.
*/
struct profile
{
    int id;
    char name[70];
    struct date birthday;
    char address[70];
    char *note;
};

/*
* @type: {int} profile_data_nitems - Total number of registered items.
* @type: {struct profile} profile_data_store[] - For storing registered data.
* @type: {struct profile} profile_data_store_ptr[] - For storing pointer data.
* @type: {FILE *} fp - Pointer for writing/reading.
*/
int profile_data_nitems = 0;
struct profile profile_data_store[10000];
struct profile *profile_data_store_ptr[10000];
FILE *fp;

int get_line(char *line);
void parse_line(char *line);

void make_profile_shadow(struct profile data_store[], struct profile *shadow[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        shadow[i] = &data_store[i];
}

/*
* Overview: Swap elements in profile_data_store array using pointer.
* @argument: {struct profile*} source - Replacement source.
* @argument: {struct profile*} destination - Replace destination.
* @return: No return
*/
void swap_p(struct profile **source, struct profile **destination)
{
    struct profile *tmp;

    tmp = *source;
    *source = *destination;
    *destination = tmp;
}

/*
* Overview: Swap elements in profile_data_store array.
* @argument: {struct profile*} source - Replacement source.
* @argument: {struct profile*} destination - Replace destination.
* @return: No return
*/
void swap(struct profile *source, struct profile *destination)
{
    struct profile tmp;

    tmp = *source;
    *source = *destination;
    *destination = tmp;
}

/*
* Overview: Quick sort profile_data_store array by id column.
* @argument: {int} low - Left edge of quick sort.
* @argument: {int} high - Right edge of quick sort.
* @return: No return
*/
void quicksort_id(int low, int high)
{
    if (low < high)
    {
        int mid = (low + high) / 2;
        int x = profile_data_store_ptr[mid]->id;
        int i = low;
        int j = high;
        while (i <= j)
        {
            while (profile_data_store_ptr[i]->id < x)
                i += 1;
            while (profile_data_store_ptr[j]->id > x)
                j -= 1;
            if (i <= j)
                if (swap_pointer)
                    swap_p(&profile_data_store_ptr[i++], &profile_data_store_ptr[j--]);
                else
                    swap(profile_data_store_ptr[i++], profile_data_store_ptr[j--]);
            else
                ;
        }
        quicksort_id(low, j);
        quicksort_id(i, high);
    }
}

/*
* Overview: Quick sort profile_data_store array by name column.
* @argument: {int} low - Left edge of quick sort.
* @argument: {int} high - Right edge of quick sort.
* @return: No return
*/
void quicksort_name(int low, int high)
{
    if (low < high)
    {
        int mid, i, j;
        char x[70];
        mid = (low + high) / 2;
        strcpy(x, profile_data_store_ptr[mid]->name);
        i = low;
        j = high;
        while (i <= j)
        {
            while (strcmp(profile_data_store_ptr[i]->name, x) < 0)
                i += 1;
            while (strcmp(profile_data_store_ptr[j]->name, x) > 0)
                j -= 1;
            if (i <= j)
                if (swap_pointer)
                    swap_p(&profile_data_store_ptr[i++], &profile_data_store_ptr[j--]);
                else
                    swap(profile_data_store_ptr[i++], profile_data_store_ptr[j--]);
            else
                ;
        }
        quicksort_name(low, j);
        quicksort_name(i, high);
    }
}

/*
* Overview: Compares two dates and returns the difference.
* @argument: {struct date *} d1 - Date1.
* @argument: {struct date *} d2 - Date2.
* @return: {int} (Date1 - Date2) - Positive or negative or zero.
*/
int compare_date(struct date *d1, struct date *d2)
{
    if (d1->y != d2->y)
        return d1->y - d2->y;
    if (d1->m != d2->m)
        return d1->m - d2->m;
    return d1->d - d2->d;
}

/*
* Overview: Quick sort profile_data_store array by birthday column.
* @argument: {int} low - Left edge of quick sort.
* @argument: {int} high - Right edge of quick sort.
* @return: No return
*/
void quicksort_birthday(int low, int high)
{
    if (low < high)
    {
        int mid = (low + high) / 2;
        struct date x = profile_data_store_ptr[mid]->birthday;
        int i = low;
        int j = high;
        while (i <= j)
        {
            while (compare_date(&profile_data_store_ptr[i]->birthday, &x) < 0)
                i += 1;
            while (compare_date(&profile_data_store_ptr[j]->birthday, &x) > 0)
                j -= 1;
            if (i <= j)
                if (swap_pointer)
                    swap_p(&profile_data_store_ptr[i++], &profile_data_store_ptr[j--]);
                else
                    swap(profile_data_store_ptr[i++], profile_data_store_ptr[j--]);
            else
                ;
        }
        quicksort_birthday(low, j);
        quicksort_birthday(i, high);
    }
}

/*
* Overview: Quick sort profile_data_store array by address column.
* @argument: {int} low - Left edge of quick sort.
* @argument: {int} high - Right edge of quick sort.
* @return: No return
*/
void quicksort_address(int low, int high)
{
    if (low < high)
    {
        int mid, i, j;
        char x[70];
        mid = (low + high) / 2;
        strcpy(x, profile_data_store_ptr[mid]->address);
        i = low;
        j = high;
        while (i <= j)
        {
            while (strcmp(profile_data_store_ptr[i]->address, x) < 0)
                i += 1;
            while (strcmp(profile_data_store_ptr[j]->address, x) > 0)
                j -= 1;
            if (i <= j)
                if (swap_pointer)
                    swap_p(&profile_data_store_ptr[i++], &profile_data_store_ptr[j--]);
                else
                    swap(profile_data_store_ptr[i++], profile_data_store_ptr[j--]);
            else
                ;
        }
        quicksort_address(low, j);
        quicksort_address(i, high);
    }
}

/*
* Overview: Quick sort profile_data_store array by note column.
* @argument: {int} low - Left edge of quick sort.
* @argument: {int} high - Right edge of quick sort.
* @return: No return
*/
void quicksort_note(int low, int high)
{
    if (low < high)
    {
        int mid, i, j;
        char x[1024];
        mid = (low + high) / 2;
        strcpy(x, profile_data_store_ptr[mid]->note);
        i = low;
        j = high;
        while (i <= j)
        {
            while (strcmp(profile_data_store_ptr[i]->note, x) < 0)
                i += 1;
            while (strcmp(profile_data_store_ptr[j]->note, x) > 0)
                j -= 1;
            if (i <= j)
                if (swap_pointer)
                    swap_p(&profile_data_store_ptr[i++], &profile_data_store_ptr[j--]);
                else
                    swap(profile_data_store_ptr[i++], profile_data_store_ptr[j--]);
            else
                ;
        }
        quicksort_note(low, j);
        quicksort_note(i, high);
    }
}

/*
* Overview: Converts lowercase letters to uppercase.
* @argument: {char *} string - The string to convert.
* @return: No return
*/
void upper(char *string)
{
    while (*string)
    {
        *string = toupper(*string);
        string++;
    }
}

/*
* Overview: Search by partial match.
* @argument: {char *} string - string to search.
* @argument: {char *} find - string to find.
* @return: No return
*/
int match(char *string, char *find)
{
    int i;
    char *string_tmp, *find_tmp;

    string_tmp = string;
    find_tmp = find;

    for (i = 0; i <= strlen(string); i++)
    {
        if (!*find_tmp)
            return 1;
        if (*string_tmp == *find && *string_tmp != *find_tmp)
            if (match(string_tmp, find))
                return 1;
        if (*string_tmp == *find_tmp)
            find_tmp++;
        else
            find_tmp = find;
        string_tmp++;
    }
    return 0;
}

/*
* Overview: Exit the program.
* @return: No return
*/
void cmd_quit(void)
{
    exit(0);
}

/*
* Overview: Output the number of registrations.
* @argument: {char} cmd - Command alphabet.
* @return: No return
*/
void cmd_check(char cmd)
{
    printf("%d profile(s)\n", profile_data_nitems);
}

/*
* Overview: Output data according to argument.
* @argument: {char} cmd - Command alphabet.
* @argument: {char *} param - Command argument.
* @return: No return
*/
void cmd_print(char cmd, char *param)
{
    int count, num = atoi(param);
    if (num == 0)
    {
        count = 0;
        while (count < profile_data_nitems)
        {
            printf("Id    : %d\n", profile_data_store_ptr[count]->id);
            printf("Name  : %s\n", profile_data_store_ptr[count]->name);
            printf("Birth : %04d-%02d-%02d\n", profile_data_store_ptr[count]->birthday.y, profile_data_store_ptr[count]->birthday.m, profile_data_store_ptr[count]->birthday.d);
            printf("Addr. : %s\n", profile_data_store_ptr[count]->address);
            printf("Comm. : %s\n\n", profile_data_store_ptr[count]->note);
            count++;
        }
    }
    else if (num > 0)
    {
        if (num > profile_data_nitems)
            num = profile_data_nitems;
        count = 0;
        while (count < num)
        {
            printf("Id    : %d\n", profile_data_store_ptr[count]->id);
            printf("Name  : %s\n", profile_data_store_ptr[count]->name);
            printf("Birth : %04d-%02d-%02d\n", profile_data_store_ptr[count]->birthday.y, profile_data_store_ptr[count]->birthday.m, profile_data_store_ptr[count]->birthday.d);
            printf("Addr. : %s\n", profile_data_store_ptr[count]->address);
            printf("Comm. : %s\n\n", profile_data_store_ptr[count]->note);
            count++;
        }
    }
    else if (num < 0)
    {
        if (num < -profile_data_nitems)
            num = -profile_data_nitems;
        count = profile_data_nitems + num;
        while (count < profile_data_nitems)
        {
            printf("Id    : %d\n", profile_data_store_ptr[count]->id);
            printf("Name  : %s\n", profile_data_store_ptr[count]->name);
            printf("Birth : %04d-%02d-%02d\n", profile_data_store_ptr[count]->birthday.y, profile_data_store_ptr[count]->birthday.m, profile_data_store_ptr[count]->birthday.d);
            printf("Addr. : %s\n", profile_data_store_ptr[count]->address);
            printf("Comm. : %s\n\n", profile_data_store_ptr[count]->note);
            count++;
        }
    }
}

/*
* Overview: Read data and register in array.
* @argument: {char} cmd - Command alphabet.
* @argument: {char *} param - Command argument.
* @return: No return
*/
void cmd_read(char cmd, char *param)
{
    char line[MAX_LINE_LEN + 1];
    fp = fopen(param, "r");
    if (fp != NULL)
    {
        while (get_line(line))
        {
            parse_line(line);
        }
    }
    else
    {
        fprintf(stderr, "Enterd file cannot be opened.\n");
    }
    fclose(fp);
}

/*
* Overview: Export registered data.
* @argument: {char} cmd - Command alphabet.
* @argument: {char *} param - Command argument.
* @return: No return
*/
void cmd_write(char cmd, char *param)
{
    int i;
    fp = fopen(param, "w");
    if (fp != NULL)
    {
        for (i = 0; i < profile_data_nitems; i++)
        {
            fprintf(fp, "%d,%s,%04d-%02d-%02d,%s,%s\n", profile_data_store_ptr[i]->id, profile_data_store_ptr[i]->name, profile_data_store_ptr[i]->birthday.y, profile_data_store_ptr[i]->birthday.m, profile_data_store_ptr[i]->birthday.d, profile_data_store_ptr[i]->address, profile_data_store_ptr[i]->note);
        }
    }
    else
    {
        fprintf(stderr, "Entered file cannot be opened.\n");
    }
    fclose(fp);
}

/*
* Overview: Search for matching data from registered data and output.
* @argument: {char} cmd - Command alphabet.
* @argument: {char *} param - Command argument.
* @return: No return
*/
void cmd_find(char cmd, char *param)
{
    int i;
    char id_tmp[9];
    char birthday_tmp[11];
    struct profile *p;
    for (i = 0; i < profile_data_nitems; i++)
    {
        p = profile_data_store_ptr[i];
        sprintf(id_tmp, "%d", p->id);
        sprintf(birthday_tmp, "%04d-%02d-%02d", p->birthday.y, p->birthday.m, p->birthday.d);
        if (
            strcmp(id_tmp, param) == 0 ||
            strcmp(p->name, param) == 0 ||
            strcmp(birthday_tmp, param) == 0 ||
            strcmp(p->address, param) == 0 ||
            strcmp(p->note, param) == 0)
        {
            printf("Id    : %d\n", profile_data_store_ptr[i]->id);
            printf("Name  : %s\n", profile_data_store_ptr[i]->name);
            printf("Birth : %04d-%02d-%02d\n", profile_data_store_ptr[i]->birthday.y, profile_data_store_ptr[i]->birthday.m, profile_data_store_ptr[i]->birthday.d);
            printf("Addr. : %s\n", profile_data_store_ptr[i]->address);
            printf("Comm. : %s\n\n", profile_data_store_ptr[i]->note);
        }
    }
}

/*
* Overview: Specify the column with an argument and sort the registered data.
* @argument: {char} cmd - Command alphabet.
* @argument: {char *} param - Command argument.
* @return: No return
*/
void cmd_sort(char cmd, char *param)
{
    switch (atoi(param))
    {
    case 1:
        quicksort_id(0, profile_data_nitems - 1);
        break;
    case 2:
        quicksort_name(0, profile_data_nitems - 1);
        break;
    case 3:
        quicksort_birthday(0, profile_data_nitems - 1);
        break;
    case 4:
        quicksort_address(0, profile_data_nitems - 1);
        break;
    case 5:
        quicksort_note(0, profile_data_nitems - 1);
        break;
    default:
        break;
    }
}

/*
* Overview: Output partial match data in array.
* @argument: {char} cmd - Command alphabet.
* @argument: {char *} param - Command argument.
* @return: No return
*/
void cmd_match(char cmd, char *param)
{
    int i;
    for (i = 0; i < profile_data_nitems; i++)
    {
        char string[2][1024];
        char find[1024];

        sprintf(string[0], "%d, %s, %d-%d-%d, %s, %s\n", profile_data_store_ptr[i]->id, profile_data_store_ptr[i]->name, profile_data_store_ptr[i]->birthday.y, profile_data_store_ptr[i]->birthday.m, profile_data_store_ptr[i]->birthday.d, profile_data_store_ptr[i]->address, profile_data_store_ptr[i]->note);
        sprintf(string[1], "%d, %s, %d-%02d-%02d, %s, %s\n", profile_data_store_ptr[i]->id, profile_data_store_ptr[i]->name, profile_data_store_ptr[i]->birthday.y, profile_data_store_ptr[i]->birthday.m, profile_data_store_ptr[i]->birthday.d, profile_data_store_ptr[i]->address, profile_data_store_ptr[i]->note);

        strcpy(find, param);

        upper(string[0]);
        upper(string[1]);
        upper(find);

        if (match(string[0], find) || match(string[1], find))
        {
            printf("Id    : %d\n", profile_data_store_ptr[i]->id);
            printf("Name  : %s\n", profile_data_store_ptr[i]->name);
            printf("Birth : %04d-%02d-%02d\n", profile_data_store_ptr[i]->birthday.y, profile_data_store_ptr[i]->birthday.m, profile_data_store_ptr[i]->birthday.d);
            printf("Addr. : %s\n", profile_data_store_ptr[i]->address);
            printf("Comm. : %s\n\n", profile_data_store_ptr[i]->note);
        }
    }
}

/*
* Overview: Calls functions when the command is input.
* @argument: {char} cmd - Command alphabet.
* @argument: {char *} param - Command argument.
* @return: No return
*/
void exec_command(char cmd, char *param)
{
    switch (cmd)
    {
    case 'Q':
        cmd_quit();
        break;
    case 'C':
        cmd_check(cmd);
        break;
    case 'P':
        cmd_print(cmd, param);
        break;
    case 'R':
        cmd_read(cmd, param);
        break;
    case 'W':
        cmd_write(cmd, param);
        break;
    case 'F':
        cmd_find(cmd, param);
        break;
    case 'S':
        cmd_sort(cmd, param);
        break;
    case 'M':
        cmd_match(cmd, param);
        break;
    default:
        fprintf(stderr, "Unregistered Command Is Entered.\n");
        break;
    }
}

/*
* Overview: Replaces c1 in the string with c2.
* @argument: {char *} str - String.
* @argument: {char} c1 - Replaced.
* @argument: {char} c2 - Replace.
* @return: {int} diff - Number of replacements.
*/
int subst(char *str, char c1, char c2)
{
    int diff = 0;
    char *p;

    p = str;
    while (*p != '\0')
    {
        if (*p == c1)
        {
            *p = c2;
            diff++;
        }
        p++;
    }
    return diff;
}

/*
* Overview: Separate string by the specified number of characters/times.
* @argument: {char *} str - String.
* @argument: {char *} ret[] - Separated string.
* @argument: {char} sep - Delimiter.
* @argument: {int} max - Maximum number to divide.
* @return: Number of divisions
*/
int split(char *str, char *ret[], char sep, int max)
{
    int count = 1;
    ret[0] = str;

    while (*str)
    {
        if (count >= max)
            break;
        if (*str == sep)
        {
            *str = '\0';
            ret[count++] = str + 1;
        }
        str++;
    }

    return count;
}

/*
* Overview: Get line from file or standard input.
* @argument: {char *} line - Full text.
* @return: Whether there is next line.
*/
int get_line(char *line)
{
    // if (fp != NULL && fgets(line, MAX_LINE_LEN + 1, fp) != NULL)
    // {
    //     subst(line, '\n', '\0');
    //     return 1;
    // }
    if (fgets(line, MAX_LINE_LEN + 1, stdin) == NULL)
    {
        return 0;
    }
    else
    {
        subst(line, '\n', '\0');
        return 1;
    }
}

/*
* Overview: New data registration.
* @argument: {struct profile *} profile_data_store - Pointer to store the new data.
* @argument: {char *} line - One line to register.
* @return: Successful or not.
*/
int new_profile(struct profile *profile_data_store, char *line)
{
    int max_line = 5, max_date = 3;
    char *ret[80] = {0}, *date[80] = {0}, sep_line = ',', sep_date = '-';

    if (split(line, ret, sep_line, max_line) != 5)
    {
        return -1;
    }

    split(line, ret, sep_line, max_line);
    split(ret[2], date, sep_date, max_date);

    profile_data_store->id = atoi(ret[0]);

    strcpy(profile_data_store->name, ret[1]);

    profile_data_store->birthday.y = atoi(date[0]);
    profile_data_store->birthday.m = atoi(date[1]);
    profile_data_store->birthday.d = atoi(date[2]);

    strcpy(profile_data_store->address, ret[3]);

    profile_data_store->note = (char *)malloc(sizeof(char) * (strlen(ret[4]) + 1));
    strcpy(profile_data_store->note, ret[4]);
    return 0;
}

/*
* Overview: Check new data registration or command.
* @argument: {char *} line - One line.
* @return: No return
*/
void parse_line(char *line)
{
    if (*line == '%')
    {
        exec_command(line[1], &line[3]);
    }
    else
    {
        new_profile(profile_data_store_ptr[profile_data_nitems++], line);
    }
}

int main(void)
{
    int sock;
    struct addrinfo hints, *result;
    struct in_addr addr;
    struct sockaddr_in sa;

    /*
    * ================================
    *  1. ソケットを作成する
    * ================================
    */

    /*
    * ----------------------
    *  1.1 サーバ情報
    * ----------------------
    */

    char *hostname = "localhost";
    char *port = "80";

    /*
    * ----------------------
    *  1.2 IPアドレスの取得
    * ----------------------
    */

    bzero((char *)&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((getaddrinfo(hostname, port, &hints, &result)) != 0)
    {
        printf("[Error] GetAddrInfo Error Occurred.\n");
        return 1;
    }

    addr.s_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr.s_addr;

    printf("> IP Address: %s\n", inet_ntoa(addr));

    /*
    * ----------------------
    *  1.3 IPアドレスの取得
    * ----------------------
    */

    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == -1)
    {
        printf("[Error] Socket Error Occurred.\n");
        freeaddrinfo(result);
        return -1;
    }

    printf("> Socket Created Successfully.\n");

    /*
    * ================================
    *  2. ソケットに名前を付ける
    * ================================
    */

    memset((char *)&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons((uint16_t)80);

    int yes = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes)) < 0)
    {
        perror("[Error] SetSockOpt Error Occurred.\n");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

    if (bind(sock, (struct sockaddr *)&sa, sizeof(sa)) == -1)
    {
        printf("[Error] Bind Error Occurred.\n");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

    printf("> Socket Bound Successfully.\n");

    /*
    * ================================
    *  3. 接続要求を待つ
    * ================================
    */

    if (listen(sock, 5) == -1)
    {
        printf("[Error] Listen Error Occurred.\n");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

    printf("> Listen Started.\n");

    /*
    * ================================
    *  4. 接続要求を受け付ける
    * ================================
    */

    socklen_t len = sizeof(struct sockaddr);
    int new_sock = accept(sock, (struct sockaddr *)&sa, &len);

    if (new_sock == -1)
    {
        printf("[Error] Accept Error Occurred.\n");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

    /*
    * ================================
    *  5. メッセージを受信する
    * ================================
    */

    char buf[MAX_LINE_LEN] = {0};
    int recv_size = recv(new_sock, (void *)buf, sizeof(buf), 0);
    if (recv_size == -1)
    {
        printf("[Error] Receive Error Occurred.\n");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

    printf("> Message Received Successfully.（%d bytes）\n", recv_size);

    /*
    * ================================
    *  @. サーバ内部処理
    * ================================
    */

    make_profile_shadow(profile_data_store, profile_data_store_ptr, 10000);
    char line[MAX_LINE_LEN + 1];

    while (get_line(line))
        parse_line(line);

    /*
    * ================================
    *  6. メッセージを送信する
    * ================================
    */

    if (send(new_sock, (const void *)buf, strlen(buf), 0) == -1)
    {
        printf("[Error] Send Error Occurred.\n");
        close(sock);
        close(new_sock);
        freeaddrinfo(result);
        return -1;
    }

    printf("> Message Sent Successfully.\n");

    close(sock);
    close(new_sock);
    freeaddrinfo(result);

    return 0;
}