#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NumberOfShops   5
#define NumberOfBuyers 3

struct selected
{
  int selectedShop;
  int selectedBuyer;
};

int buyers[NumberOfBuyers]; //3 покупателя
int shops[NumberOfShops]; //5 магазинов
pthread_mutex_t busy[NumberOfShops]; //5 магазинов могут быть заняты

//функции погрузчика
void  get_load_busy (struct selected* id);
void  get_load_free (struct selected* id);
void* load (void* arg);
//функции покупателей
void  get_buyer_busy (struct selected* id);
void  get_buyer_free (struct selected* id);
void* buy (void* arg);

int main(void)
{
  time_t t;
  srand((unsigned) time(&t));
  for(int i=0;i<5;i++) //инициализация первичных товаров
  {
    shops[i]=190+rand()%11;
  }
  for(int i=0;i<3;i++) //инициализация первичных покупателей
  {
    buyers[i]=1000500+rand()%251; //проверял на 1000, чтобы быстрее было
  }
  pthread_t buyerThread[3];
  pthread_t loader;
  struct selected arg;

  while(buyers[0]>0 || buyers[1]>0 || buyers[2]>0)
  {
  arg.selectedShop=rand()%6; //индекс выбранного магазина
  arg.selectedBuyer=rand()%3; //индекс выбранного покупателя
  pthread_create(&loader, NULL, load, &arg);
  pthread_create(&buyerThread[arg.selectedBuyer], NULL, buy, &arg);
  }
  return 0;
}

void
get_load_busy (struct selected* id)
{
  pthread_mutex_lock (&busy[id->selectedShop]);
  printf ("Магазин %d: Теперь занят погрузчиком\n", id->selectedShop);
}

void
get_buyer_busy (struct selected* id)
{
  pthread_mutex_lock (&busy[id->selectedShop]);
  printf ("Магазин %d: Теперь занят покупателем %d\n", id->selectedShop, id->selectedBuyer);
}

void
get_load_free (struct selected* id)
{
  printf ("Магазин %d: Теперь свободен от погрузчика\n", id->selectedShop);
  pthread_mutex_unlock (&busy[id->selectedShop]);
}

void
get_buyer_free (struct selected* id)
{
  printf ("Магазин %d: Теперь свободен от покупателя %d\n", id->selectedShop, id->selectedBuyer);
  pthread_mutex_unlock (&busy[id->selectedShop]);
}

void*
load (void* arg)
{
  get_load_busy(arg);
  struct selected* id = (struct selected*) arg;
  printf("Погрузчик начал грузить товар в магазин %d\n", id->selectedShop);
  shops[id->selectedShop]+=150;
  printf("Теперь в магазине %d стало товара: %d\n", id->selectedShop, shops[id->selectedShop]);
  printf("Погрузчик уснул\n");
  sleep(2);
  get_load_free(arg);
}

void*
buy (void* arg)
{
  get_buyer_busy(arg);
  struct selected* id = (struct selected*) arg;
  if(buyers[id->selectedBuyer]<=0)
  {
    printf("Покупатель %d закончил закупку\n", id->selectedBuyer);
    get_buyer_free(arg);
    pthread_exit(NULL);
  }
  else
  {
  printf("Покупатель %d начал покупать товар в магазине %d\n", id->selectedBuyer, id->selectedShop);
  buyers[id->selectedBuyer]-=shops[id->selectedShop];
  printf("Покупатель %d ещё имеет потребность: %d\n", id->selectedBuyer, buyers[id->selectedShop]);
  shops[id->selectedShop]=0;
  printf("Покупатель %d очистил магазин %d\n", id->selectedBuyer, id->selectedShop);
  printf("Покупатель %d уснул\n", id->selectedBuyer);
  sleep(1);
  get_buyer_free(arg);
  }
}
