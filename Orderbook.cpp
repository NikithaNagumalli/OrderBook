#include <iostream>
#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <limits>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <variant>
#include <optional>
#include <tuple>
#include <format>

enum class OrderType {
    GoodTillCancel,
    FillAndKill
};

enum class Side {
    Buy,
    Sell
};

using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

struct LevelInfo {
    Price price_;
    Quantity quantity_;
};

using LevelInfos = std::vector<LevelInfo>;

class OrderbookLevelInfos {
    public:
        OrderbookLevelInfos(const LevelInfos& bids, const LevelInfos& asks) {
            bids_ = bids;
            asks_ = asks;
        }

        const LevelInfos& getBids() const { return bids_; }
        const LevelInfos& getAsks() const { return asks_; }

    private:
        LevelInfos bids_;
        LevelInfos asks_;
};

class Order {
    public:
        Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity) {
            orderType_ = orderType;
            orderId_ = orderId;
            side_ = side;
            price_ = price;
            initialQuantity_ = quantity;
            remainingQuantity_ = quantity;
        }
        OrderId getOrderId() const { return orderId_; }
        Side getSide() const { return side_; }
        Price getPrice() const { return price_; }
        Quantity getInititalQuantity() const { return initialQuantity_; }
        Quantity getRemainingQuantity() const { return remainingQuantity_; }
        Quantity getFilledQuantity() const { return getInititalQuantity() - getRemainingQuantity(); };
        void Fill(Quantity quantity) {
            if (quantity > getRemainingQuantity()) {
                throw std::logic_error("Order ({" + std::to_string(getOrderId()) + "}) cannot be filled more than its remaining quantity.");
            }
            remainingQuantity_-= quantity;
        }
    private:
        OrderType orderType_;
        OrderId orderId_;
        Side side_;
        Price price_;
        Quantity initialQuantity_;
        Quantity remainingQuantity_;
};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

class OrderModify {
    public: 
        OrderModify(OrderId orderId, Side side, Price price, Quantity quantity) {
            orderId_ = orderId;
            side_ = side;
            price_ = price;
            quantity_ = quantity;
        }
        OrderId getOrderId() const { return orderId_; }
        Side getSide() const { return side_; }
        Price getPrice() const { return price_; }
        Quantity getQuantity() const { return quantity_; }

        OrderPointer ToOrderPointer(OrderType type) const {
            return std::make_shared<Order>(type, getOrderId(), getSide(), getPrice(), getQuantity());
        }

    private:
        OrderId orderId_;
        Side side_;
        Price price_;
        Quantity quantity_;
};

int main() {
    return 0;
}