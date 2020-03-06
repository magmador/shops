#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NumberOfShops   5
#define NumberOfBuyers 3

int buyers[NumberOfBuyers]; //3 покупателя
int shops[NumberOfShops]; //5 магазинов
pthread_mutex_t busy[NumberOfShops]; //5 магазинов могут быть заняты

//функции погрузчика
void  get_load_busy (int shopId);
void  get_load_free (int shopId);
void* load ();

//функции покупателей
void  get_buyer_busy (int shopId, int buyerId);
void  get_buyer_free (int shopId, int buyerId);
void* buy (void* buyerId);

int main(void)
{
  time_t t;
  srand((unsigned) time(&t));
  int id[NumberOfBuyers];

  for(int i=0;i<5;i++) //инициализация первичных товаров в магазинах
  {
    pthread_mutex_unlock(&busy[i]);
    shops[i]=190+rand()%11;
  }
  for(int i=0;i<3;i++) //инициализация потребности первичных покупателей
  {
    buyers[i]=1000;//+rand()%251; //проверял на 1000, чтобы быстрее было
    id[i]=i;
  }

  pthread_t loader;
  pthread_t buyersThread[NumberOfBuyers];
  pthread_create(&loader, NULL, load, NULL);
  for(int i=0;i<3;i++)
    pthread_create(&buyersThread[i], NULL, buy, &id[i]);
  for(int i=0;i<3;i++)
    pthread_join(buyersThread[i], NULL);
  pthread_cancel(loader);
  return 0;
}

void
get_load_busy (int shopId)
{
  pthread_mutex_lock (&busy[shopId]);
  printf ("Магазин %d: Теперь занят погрузчиком\n", shopId);
}

void
get_load_free (int shopId)
{
  printf ("Магазин %d: Теперь свободен от погрузчика\n", shopId);
  pthread_mutex_unlock (&busy[shopId]);
}

void*
load ()
{
  while(1)
  {
    int shopId = rand()%5;
    get_load_busy(shopId);
    printf("Погрузчик начал грузить товар в магазин %d\n", shopId);
    shops[shopId]+=150;
    printf("Теперь в магазине %d стало товара: %d\n", shopId, shops[shopId]);
    printf("Погрузчик уснул\n");
    get_load_free(shopId);
    sleep(2);
  }
}

void
get_buyer_busy (int buyerId, int shopId)
{
  pthread_mutex_lock (&busy[shopId]);
  printf ("Магазин %d: Теперь занят покупателем %d\n", shopId, buyerId);
}

void
get_buyer_free (int buyerId, int shopId)
{
  printf ("Магазин %d: Теперь свободен от покупателя %d\n", shopId, buyerId);
  pthread_mutex_unlock (&busy[shopId]);
}

void*
buy (void* buyerId)
{
  int shopId;
  while(1)
  {
    shopId=rand()%5; //покупатель идёт в случайный магазин
    if(buyers[*((int*)buyerId)]<=0)
    {
      //после того, как у покупателя закончилась потребность, поток завершается
      pthread_exit(NULL);
    }
    else
    {
      get_buyer_busy(*((int *)buyerId), shopId); //лочит мьютекс
      printf("Покупатель %d начал покупать товар в магазине %d\n", *((int *)buyerId), shopId);
      buyers[*((int *)buyerId)]-=shops[shopId];
      printf("Покупатель %d ещё имеет потребность: %d\n", *((int *)buyerId), buyers[shopId]);
      shops[shopId]=0;
      printf("Покупатель %d очистил магазин %d\n", *((int *)buyerId), shopId);
      printf("Покупатель %d уснул\n", *((int *)buyerId));
      get_buyer_free(*((int *)buyerId), shopId); //анлочит мьютек
      sleep(1); //и засыпает
    }
  }
}
