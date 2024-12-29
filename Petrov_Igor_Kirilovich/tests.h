#include <cassert>

#include "candle.h"

/*Тут я написал максимально примитивно булевые сравнения не стал усложнять как просили. Деталей было не написанно никаких были мысли добавить
гугл тесты которые не разу не использовал но для чтобы усложнить код и сделать его более интересным плюс можно добавить макросы
и функции с выводом сообщений, не добавил так как я боюсь что могут быть дополнительные сложности из-за многопоточности
(которую я пока не успел идеально выучить) которые я не смогу вовремя решить чтобы моя работа не затерялась среди всех*/

bool TestBodyContains() {
    Candle candle(10, 20, 5, 15);
    assert(candle.BodyContains(12) == true);
    assert(candle.BodyContains(10) == true);

    candle = Candle(20, 30, 15, 10);
    assert(candle.BodyContains(9) == false);

    return true;
}

bool TestIsContains() {
    Candle candle(10, 20, 5, 15);
    assert(candle.IsContains(15) == true);
    assert(candle.IsContains(5) == true);
    assert(candle.IsContains(21) == false);

    return true;
}

bool TestGetShadows() {
    Candle candle(10, 20, 5, 15);
    assert(candle.GetShadows() == 15);

    candle = Candle(10, 10, 10, 10);
    assert(candle.GetShadows() == 0);

    candle = Candle(-10, 10, -15, 5);
    assert(candle.GetShadows() == 25);

    return true;
}

bool TestGetBody() {
    Candle candle(10, 20, 5, 15);
    assert(candle.GetBody() == 5);

    candle = Candle(20, 10, 5, 10);
    assert(candle.GetBody() == 10);

    candle = Candle(10, 10, 10, 10);
    assert(candle.GetBody() == 0);

    return true;
}

bool TestIsRed() {
    Candle candle(20, 30, 10, 10);
    assert(candle.IsRed() == true);

    candle = Candle(10, 20, 5, 15);
    assert(candle.IsRed() == false);

    candle = Candle(10, 10, 10, 10);
    assert(candle.IsRed() == false);

    return true;
}

bool TestIsGreen() {
    Candle candle(10, 20, 5, 15);
    assert(candle.IsGreen() == true);

    candle = Candle(20, 10, 5, 10);
    assert(candle.IsGreen() == false);

    candle = Candle(10, 10, 10, 10);
    assert(candle.IsGreen() == false);

    return true;
}


