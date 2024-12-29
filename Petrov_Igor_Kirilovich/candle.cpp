#include <cmath>

#include "candle.h"

Candle::Candle(Price open, Price high, Price low, Price close)
	: open_(open)
	, high_(high)
	, low_(low)
	, close_(close) {}

bool Candle::BodyContains(Price price) const noexcept
{
	if (IsGreen())
	{
		return price >= open_ && price <= close_;
	}
	else
	{
		return price >= close_ && price <= open_;
	}
}

bool Candle::IsContains(Price price) const noexcept
{
	return price >= low_ && price <= high_;
}

double Candle::GetShadows() const noexcept
{
	return std::abs(low_ - high_);
}

double Candle::GetBody() const noexcept
{
	return std::abs(open_ - close_);
}

bool Candle::IsGreen() const noexcept
{
	return close_ > open_;
}

bool Candle::IsRed() const noexcept
{
	return close_ < open_;
}
