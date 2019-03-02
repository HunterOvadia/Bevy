#define BEVY_DOG_USE_BEVY_LOG
#define BEVY_DOG_QUIET
#include "bevy/bevy_dog.h"

typedef struct Person
{
    char *name;
    int age;
    float weight;
}
Person;


int
main(int argc, char **argv)
{
    Person *billy[100];
    for(int i = 0; i < 100; ++i)
    {
        billy[i] = bevy_malloc(sizeof(Person));
    }


    Person *bobby = bevy_calloc(1, sizeof(Person));

    for(int i = 0; i < 100; ++i)
    {
            bevy_free(billy[i]);
    }
    bevy_analyze();
    system("pause");
    return 0;
}