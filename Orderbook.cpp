#include <iostream>
// #include <stdint.h>
// #include <sys/types.h>
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
        Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
            : orderType_ {orderType}
            , orderId_ {orderId}
            , side_ {side}
            , price_ {price}
            , initialQuantity_ {quantity}
            , remainingQuantity_ {quantity}
        {}
        OrderId getOrderId() const { return orderId_; }
        Side getSide() const { return side_; }
        Price getPrice() const { return price_; }
        Quantity getInititalQuantity() const { return initialQuantity_; }
        Quantity getRemainingQuantity() const { return remainingQuantity_; }
        Quantity getFilledQuantity() const { return getInititalQuantity() - getRemainingQuantity(); };
        bool IsFilled() const { return getRemainingQuantity() == 0; }
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
        OrderModify(OrderId orderId, Side side, Price price, Quantity quantity) 
            : orderId_ {orderId}
            , side_ {side}
            , price_ {price}
            , quantity_ {quantity}
        {}
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

struct TradeInfo {
    OrderId orderId_;
    Price price_;
    Quantity quantity_;
};

class Trade {
    public: 
        Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade)
            : bidTrade_ {bidTrade}
            , askTrade_ {askTrade}
        {}
        
        const TradeInfo& GetBidTrade() const { return bidTrade_; }
        const TradeInfo& GetAskTrade() const { return askTrade_; }
    
    private:
        TradeInfo bidTrade_;
        TradeInfo askTrade_;
};

using Trades = std::vector<Trade>;

class OrderBook {
    private:
        struct OrderEntry {
            OrderPointer order_ {nullptr};
            OrderPointers::iterator location_;
        };

        std::map<Price, OrderPointers, std::greater<Price>> bids_; // ordered from greatest price to least price
        std::map<Price, OrderPointers, std::less<Price>> asks_; // ordered from least price to greatest price
        std::unordered_map<OrderId, OrderEntry> orders_;

        bool CanMatch(Side side, Price price) const {
            if (side == Side::Buy) {
                if (asks_.empty()) {
                    return false;
                }

                const auto& [bestAsk, _] = *asks_.begin();
                return price >= bestAsk;
            } else if (side == Side::Sell) {
                if (bids_.empty()) {
                    return false;
                }

                const auto& [bestBid, _] = *bids_.begin();
                return price <= bestBid;
            }
        }

        Trades MatchOrders() {
            Trades trades;
            trades.reserve(orders_.size());

            while (true) {
                if (bids_.empty() || asks_.empty()) {
                   break;
                }

                auto& [bidPrice, bids] = *bids_.begin();
                auto& [askPrice, asks] = *asks_.begin();

                if (bidPrice < askPrice) {
                    break;
                }

                while(bids.size() && asks.size()) {
                    auto& bid = bids.front();
                    auto& ask = asks.front();

                    Quantity quantity = std::min(bid->getRemainingQuantity(), ask->getRemainingQuantity());

                    bid->Fill(quantity);
                    ask->Fill(quantity);

                    if (bid->IsFilled()) {
                        bids.pop_front();
                        orders_.erase(bid->getOrderId());
                    }

                    if (ask->IsFilled()) {
                        asks.pop_front();
                        orders_.erase(ask->getOrderId());
                    }

                    if (bids_.empty()) {
                        bids_.erase(bidPrice);
                    }

                    if (asks_.empty()) {
                        asks_.erase(askPrice);
                    }

                    // Trade trade = Trade {

                    // }
                    // trades.push_back()

                }
            }

            return trades;
        }
};

int main() {
    return 0;
}