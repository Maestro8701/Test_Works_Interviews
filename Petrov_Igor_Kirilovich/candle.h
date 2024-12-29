#pragma once

using Price = double;

class Candle
{
private:
	Price open_;
	Price high_;
	Price low_;
	Price close_;
	
public:
	Candle() = default;
	Candle(Price open, Price high, Price low, Price close);

	//находится ли цена внутри тела этой свечи
	bool BodyContains(Price price) const noexcept;

	//находится ли цена внутри тела и теней
	//другими словами, касается ли цена
	//тела или теней свечи
	bool IsContains(Price price) const noexcept;

	//возвращает размер целой свечи, включая тени
	double GetShadows() const noexcept;

	//возвращает размер тела свечи
	double GetBody() const noexcept;

	bool IsGreen() const noexcept;
	bool IsRed() const noexcept;

};
