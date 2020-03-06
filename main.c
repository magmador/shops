#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define NUMBER_OF_SHOPS  5
#define NUMBER_OF_BUYERS 3

#define MIN_GOODS_IN_SHOP 190
#define GOODS_SCATTER     10
#define BUYER_SCATTER     250
#define BUYER_NEEDED      1000

#define ID_TO_INDEX(X) *((int *)X)

int buyers[NUMBER_OF_BUYERS];          //3 покупателя
int shops[NUMBER_OF_SHOPS];            //5 магазинов
pthread_mutex_t busy[NUMBER_OF_SHOPS]; //5 магазинов могут быть заняты

//функции погрузчика
void get_load_busy(int shopId);
void get_load_free(int shopId);
void *load();

//функции покупателей
void get_buyer_busy(int shopId, int buyerId);
void get_buyer_free(int shopId, int buyerId);
void *buy(void *buyerId);

int main(void)
{
	time_t t;
	srand((unsigned int)time(&t));

	for (int i = 0; i < NUMBER_OF_SHOPS; i++) //инициализация первичных товаров
	{
		pthread_mutex_unlock(&busy[i]);
		shops[i] = MIN_GOODS_IN_SHOP + rand() % GOODS_SCATTER;
	}

	int id[NUMBER_OF_BUYERS];
	for (int i = 0; i < NUMBER_OF_BUYERS; i++) //инициализация потребности первичных покупателей
	{
		buyers[i] = BUYER_NEEDED + rand() % BUYER_SCATTER; //проверял на 1000, чтобы быстрее было
		id[i] = i;
	}

	pthread_t loader;
	pthread_create(&loader, NULL, load, NULL);

	pthread_t buyersThread[NUMBER_OF_BUYERS];
	for (int i = 0; i < NUMBER_OF_BUYERS; i++)
		pthread_create(&buyersThread[i], NULL, buy, &id[i]);

	for (int i = 0; i < NUMBER_OF_BUYERS; i++)
		pthread_join(buyersThread[i], NULL);

	pthread_cancel(loader);
	return EXIT_SUCCESS;
}

void get_load_busy(int shopId)
{
	pthread_mutex_lock(&busy[shopId]);
#ifdef DEBUG
	printf("Магазин %d: Теперь занят погрузчиком\n", shopId);
#endif //!DEBUG
}

void get_load_free(int shopId)
{
#ifdef DEBUG
	printf("Магазин %d: Теперь свободен от погрузчика\n", shopId);
#endif //!DEBUG
	pthread_mutex_unlock(&busy[shopId]);
}

void *load()
{
	while (1)
	{
		int shopToLoad = rand() % NUMBER_OF_SHOPS;

		get_load_busy(shopToLoad);
#ifdef DEBUG
		printf("Погрузчик начал грузить товар в магазин %d\n", shopToLoad);
#endif //!DEBUG

		shops[shopToLoad] += BUYER_NEEDED;
#ifdef DEBUG
		printf("Теперь в магазине %d стало товара: %d\n", shopToLoad, shops[shopToLoad]);
#endif //!DEBUG

		get_load_free(shopToLoad);
#ifdef DEBUG
		printf("Погрузчик уснул\n");
#endif //!DEBUG
		sleep(2);
	}
}

void get_buyer_busy(int buyerId, int shopId)
{
	pthread_mutex_lock(&busy[shopId]);
#ifdef DEBUG
	printf("Магазин %d: Теперь занят покупателем %d\n", shopId, buyerId);
#endif //!DEBUG
}

void get_buyer_free(int buyerId, int shopId)
{
#ifdef DEBUG
	printf("Магазин %d: Теперь свободен от покупателя %d\n", shopId, buyerId);
#endif //!DEBUG
	pthread_mutex_unlock(&busy[shopId]);
}

void *buy(void *buyerId)
{
	int shopToGo;
	while (1)
	{
		shopToGo = rand() % NUMBER_OF_SHOPS; //покупатель идёт в случайный магазин
		if (buyers[ID_TO_INDEX(buyerId)] <= 0)
		{
			//после того, как у покупателя закончилась потребность, поток завершается
			pthread_exit(NULL);
		}
		else
		{
			get_buyer_busy(ID_TO_INDEX(buyerId), shopToGo); //лочит мьютекс
#ifdef DEBUG
			printf("Покупатель %d начал покупать товар в магазине %d\n", ID_TO_INDEX(buyerId), shopToGo);
#endif //!DEBUG

			buyers[ID_TO_INDEX(buyerId)] -= shops[shopToGo];
#ifdef DEBUG
			printf("Покупатель %d ещё имеет потребность: %d\n", ID_TO_INDEX(buyerId), buyers[shopToGo]);
#endif //!DEBUG

			shops[shopToGo] = 0;
#ifdef DEBUG
			printf("Покупатель %d очистил магазин %d\n", ID_TO_INDEX(buyerId), shopToGo);
#endif //!DEBUG

			get_buyer_free(ID_TO_INDEX(buyerId), shopToGo); //анлочит мьютек
			printf("Покупатель %d уснул\n", ID_TO_INDEX(buyerId));
			sleep(1);                                      //и засыпает
		}
	}
}
