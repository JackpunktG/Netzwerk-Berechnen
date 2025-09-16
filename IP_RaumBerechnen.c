#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

// OS detection
#ifdef _WIN32
char *windows_getline()
{
    char *line = malloc(256);
    if (line == NULL)
        return NULL;

    if (fgets(line, 256, stdin) != NULL)
    {
        return line;
    }
    else
    {
        free(line);
        return NULL;
    }
}
#endif

bool ipCorrect = false;
bool round1 = false;

void *IP_raum_berechnen(uint8_t *ip, uint8_t subNetz, uint8_t *NetzID, uint8_t *BC)
{
    unsigned int schrittWerte;
    unsigned int x = subNetz;
    uint32_t subNetznummer;
    uint8_t y = 0;

    while (x % 8 != 0)
    {
        x++;
        y++;
    }

    schrittWerte = (int)pow(2, y);

    if (subNetz <= 8)
        x = 0;
    else if (subNetz <= 16)
        x = 1;
    else if (subNetz <= 24)
        x = 2;
    else
        x = 3;

    subNetznummer = ip[x] / schrittWerte;

    int netzID = schrittWerte * subNetznummer;
    int Broadcast = netzID + schrittWerte - 1;

    if (!round1)
    {
        printf("\nGesamt IP Raum!!\n");
        round1 = true;
    }
    for (int i = 0; i < 4; i++)
    {
        if (i == x)
        {
            printf("%u", (uint8_t)netzID);
            NetzID[i] = (uint8_t)netzID;
        }
        else
        {
            if (i < x)
            {
                printf("%u", ip[i]);
                NetzID[i] = ip[i];
            }
            else
            {
                printf("0");
                NetzID[i] = 0;
            }
        }
        if (i != 3)
            printf(".");
    }
    printf(" - ");
    for (int i = 0; i < 4; i++)
    {
        if (i == x)
        {
            printf("%u", (uint8_t)Broadcast);
            BC[i] = (uint8_t)Broadcast;
        }
        else
        {
            if (i < x)
            {
                printf("%u", ip[i]);
                BC[i] = ip[i];
            }
            else
            {
                printf("255");
                BC[i] = 255;
            }
        }
        if (i != 3)
            printf(".");
    }
    printf("\n");
    printf("NetzID    - Broadcast\n");
    printf("   +1   -   -1         *Nutzbaren IP's\n");
}

bool isPowerOfTwo(int n)
{
    return n > 0 && (n & (n - 1)) == 0;
}

void subnetz_next(uint8_t *nextNetzID, uint8_t *perviousBC)
{
    nextNetzID[3] = perviousBC[3] + 1;
    if (nextNetzID[3] < perviousBC[3])
    {
        nextNetzID[2] = perviousBC[2] + 1;
        if (nextNetzID[2] < perviousBC[2])
        {
            nextNetzID[1] = perviousBC[1] + 1;
            if (nextNetzID[1] < perviousBC[1])
            {
                nextNetzID[0] = perviousBC[0] + 1;
                if (nextNetzID[0] < perviousBC[0])
                {
                    printf("ERROR is finding next NetzID");
                }
            }
        }
    }
}

void subnetz_berechnen(uint8_t *ip, uint8_t subNetz, int subNetzMenge, uint8_t *NetzID, uint8_t *BC)
{
    long IPAddressen = 1;

    for (int i = 0; i < 4; i++)
    {
        IPAddressen *= (BC[i] - NetzID[i] + 1);
    }

    long proSubnetzIP = IPAddressen / subNetzMenge;

    int menge = subNetzMenge;

    while (!isPowerOfTwo(menge))
        menge++;

    uint8_t subNetzBits = 0;
    for (;;)
    {
        subNetzBits++;
        if (((int)pow(2, subNetzBits)) == menge)
            break;
    }

    subNetzBits += subNetz;

    uint8_t NetzIDSubnetz[4];
    for (int i = 0; i < 4; i++)
    {
        NetzIDSubnetz[i] = NetzID[i];
    }

    for (int i = 0; i < menge; i++)
    {
        printf("--------------------\nNetzraum %d\n", i + 1);
        IP_raum_berechnen(NetzIDSubnetz, subNetzBits, NetzID, BC);
        subnetz_next(NetzIDSubnetz, BC);
        printf("--------------------\n");
    }
}

void set_IP(char *IP, uint8_t *ip)
{
    size_t len = strlen(IP);
    if (len > 0 && IP[len - 1] == '\n')
    {
        IP[len - 1] = '\0';
    }

    char *token;
    token = strtok(IP, ".");

    if (token != NULL)
    {
        ip[0] = (uint8_t)atoi(token);
    }
    else
    {
        printf("INVALID IP\n");
        return;
    }

    for (int i = 1; i < 4; i++)
    {
        token = strtok(NULL, ".");
        if (token != NULL)
        {
            ip[i] = (uint8_t)atoi(token);
        }
        else
        {
            printf("INVALID IP\n");
            return;
        }
    }
    printf("IP correct\n");
    ipCorrect = true;
}

void print_raum(uint8_t *NetzID, uint8_t *BC)
{
    for (int i = 0; i < 4; i++)
    {
        printf("%u ", NetzID[i]);
    }
    printf("\n");
    for (int i = 0; i < 4; i++)
    {
        printf("%u ", BC[i]);
    }
}

void set_IP_subnetz(uint8_t *ip, uint8_t *subNetz)
{
    while (!ipCorrect)
    {
        printf("\nIP Address: ");

#ifdef _WIN32
        char *ipSt = windows_getline();
        if (ipSt != NULL)
        {
            set_IP(ipSt, ip);
            free(ipSt);
        }
        else
        {
            printf("INVALID IP\n");
        }
#else
        char *ipSt = NULL;
        size_t len = 0;

        getline(&ipSt, &len, stdin);

        if (len > 0)
        {
            set_IP(ipSt, ip);
            free(ipSt);
        }
        else
        {
            printf("INVALID IP\n");
            free(ipSt);
        }
#endif
    }
    ipCorrect = false;
    printf("Classless Inter-Domain Routing (CIDR) läenge?\n");
    int temp;
    scanf("%d", &temp);
    *subNetz = (uint8_t)temp;
    getchar();
}

int main()
{
    bool running = true;
    printf("*** Netzwerk Berechen ****\n");
    int options;

    while (running)
    {
        options = 0;

        printf("\n1 - IP-raum Berechnen\n2 - Subnetz\n9 - exit\n");
        scanf("%d", &options);
        getchar();

        if (options == 1)
        {
            uint8_t NetzID[4];
            uint8_t BC[4];
            uint8_t subNetz;
            uint8_t ip[4];
            set_IP_subnetz(ip, &subNetz);
            IP_raum_berechnen(ip, subNetz, NetzID, BC);
        }
        else if (options == 2)
        {
            uint8_t NetzID[4];
            uint8_t BC[4];
            uint8_t subNetz;
            uint8_t ip[4];
            int subNetzMenge;
            set_IP_subnetz(ip, &subNetz);

            printf("Im wieviel Subnetz willst du die Raum verteilen?\n");
            scanf("%d", &subNetzMenge);
            getchar();

            IP_raum_berechnen(ip, subNetz, NetzID, BC);
            subnetz_berechnen(ip, subNetz, subNetzMenge, NetzID, BC);
        }
        else if (options == 9)
        {
            printf("Ciao!!\n");
            running = false;
        }
        else
            printf("UNGUELTIG Eingeben\n");

        options = 0;
        round1 = false;
    }
}