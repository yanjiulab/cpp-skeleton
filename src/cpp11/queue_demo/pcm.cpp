#include "concurrentqueue/concurrentqueue.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <chrono>
#include <random>

using namespace moodycamel;

// 定义生产消费的元素结构
struct Item {
    int value;
    std::string text;
    double data;
};

// 全局控制变量：是否继续生产（原子类型保证线程安全）
std::atomic<bool> produce(true);
// 统计生产/消费的总数（用于验证数据完整性）
std::atomic<int> totalProduced(0);
std::atomic<int> totalConsumed(0);

// 生成随机字符串（用于Item的text字段）
std::string generateRandomString(int length = 8) {
    const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, chars.size() - 1);
    
    std::string str;
    for (int i = 0; i < length; ++i) {
        str += chars[dis(gen)];
    }
    return str;
}

// 补全：生产Item的函数
Item produceItem() {
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
    std::uniform_int_distribution<> intDis(1, 1000);    // 随机int值
    std::uniform_real_distribution<> doubleDis(0.0, 100.0); // 随机double值

    Item item;
    item.value = intDis(gen);
    item.text = generateRandomString();
    item.data = doubleDis(gen);
    
    totalProduced.fetch_add(1, std::memory_order_relaxed);
    return item;
}

// 补全：消费Item的函数
void consumeItem(const Item& item) {
    // 模拟消费逻辑：打印Item内容（实际场景可替换为业务逻辑）
    std::cout << "[Consumer " << std::this_thread::get_id() 
              << "] Consumed - Value: " << item.value 
              << ", Text: " << item.text 
              << ", Data: " << item.data << std::endl;
    
    // 模拟消费耗时（避免消费过快，更贴近真实场景）
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    totalConsumed.fetch_add(1, std::memory_order_relaxed);
}

int main() {
    // 初始化并发队列（默认参数，也可指定容量：ConcurrentQueue<Item> q(1024);）
    ConcurrentQueue<Item> q;
    const int ProducerCount = 2;  // 生产者线程数
    const int ConsumerCount = 2;  // 消费者线程数
    
    std::thread producers[ProducerCount];
    std::thread consumers[ConsumerCount];
    std::atomic<int> doneProducers(0);  // 已完成的生产者数
    std::atomic<int> doneConsumers(0);  // 已完成的消费者数

    // 启动生产者线程
    for (int i = 0; i < ProducerCount; ++i) {
        producers[i] = std::thread([&]() {
            // 生产逻辑：持续生产直到produce被置为false
            while (produce.load(std::memory_order_acquire)) {
                q.enqueue(produceItem());
                // 模拟生产耗时，避免生产过快导致队列爆满
                std::this_thread::sleep_for(std::chrono::microseconds(5000));
            }
            // 标记当前生产者完成
            doneProducers.fetch_add(1, std::memory_order_release);
            std::cout << "[Producer " << std::this_thread::get_id() << "] Finished producing" << std::endl;
        });
    }

    // 启动消费者线程
    for (int i = 0; i < ConsumerCount; ++i) {
        consumers[i] = std::thread([&]() {
            Item item;
            bool itemsLeft = true;

            do {
                // 检查是否还有生产者在运行，或队列中还有未消费的元素
                itemsLeft = (doneProducers.load(std::memory_order_acquire) != ProducerCount);
                
                // 尝试消费队列中的元素（循环消费，直到队列为空）
                while (q.try_dequeue(item)) {
                    consumeItem(item);
                    itemsLeft = true;  // 只要消费到元素，就说明还有数据待处理
                }

                // 短暂休眠，避免空轮询占用CPU
                if (itemsLeft) {
                    std::this_thread::yield();
                }

            } while (itemsLeft);

            // 标记当前消费者完成
            int finished = doneConsumers.fetch_add(1, std::memory_order_acq_rel) + 1;
            std::cout << "[Consumer " << std::this_thread::get_id() << "] Finished consuming (Total finished: " << finished << ")" << std::endl;
        });
    }

    // 主线程：控制生产时长（运行1秒后停止生产）
    std::cout << "Main thread: Running producers for 1 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 停止生产（置为false，所有生产者线程会退出循环）
    produce.store(false, std::memory_order_release);
    std::cout << "Main thread: Stopping producers..." << std::endl;

    // 等待所有生产者线程结束
    for (int i = 0; i < ProducerCount; ++i) {
        producers[i].join();
    }
    std::cout << "All producers have finished." << std::endl;

    // 等待所有消费者线程结束
    for (int i = 0; i < ConsumerCount; ++i) {
        consumers[i].join();
    }
    std::cout << "All consumers have finished." << std::endl;

    // 输出统计信息，验证生产/消费数据一致性
    std::cout << "\n===== Final Statistics =====" << std::endl;
    std::cout << "Total items produced: " << totalProduced.load() << std::endl;
    std::cout << "Total items consumed: " << totalConsumed.load() << std::endl;
    if (totalProduced == totalConsumed) {
        std::cout << "✅ All items were successfully consumed!" << std::endl;
    } else {
        std::cout << "❌ Mismatch: Some items were not consumed!" << std::endl;
    }

    return 0;
}
