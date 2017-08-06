# 设计
* 全面认真的设计消息，定义系统各个组件之间公用的各类消息
* 明确各个消息对应的内部组件架构，例如Instrument，内部需要对Instrument进行管理
* 定义这个系统的架构，例如使用数据流模式+过滤器模式，考虑实现语言和库
* 在完成上述的基础上，定义插件架构及插件接口

# 消息
## 分类
 * 市场类消息
   * Ask/Bid/Trade (Snapshot)    Update  4
   * Level2        (Snapshot)    Update  2
   * News                        Update  N
   * Fundamental                 Update  N
   * Feed Status                 Update  2
 * 交易类消息
   * Order                      Command
   * ExecutionReport            Update
   * AccountReport              Update
   * Query OpenOrder            Query
   * Query HistoricalOrder      Query
   * Position                   Request/Update
 * 交易品种类消息
   * Instrument Definition      Request
   * Exchange                   Request
 * 历史类消息
   * 历史数据                    Request
 * Login消息
   * Logon/LogOff/HEARTBEAT     Request


# 组件和功能
## Instrument & InstrumentManager
  * Instrument定义了交易品种，由Symbol和Exchange唯一确定(或者加上Type)
## Order & OrderManager
  * 管理Order的生命周期, 通过register方法给Order.Id一个唯一值
  * 通过OCA属性管理OCA订单
  * 提供Send/Cancel/Replace方法，主要是生成ExecutionCommand
  * 接收ExecutionReport消息，并更新相应的Order信息
  * 接收AccountReport消息
  * 管理持久化，通过OrderServer保存
  * 路由ExecutionCommand到RiskManager，并接收反馈、生成相应的ExecutionRepor
  * 提供查询方法，例如通过ProviderOrderId查询订单等
## AccountData && AcountDataManager
## Data & DataManager
## BarFactory
  * 提供virtual接口，可以扩展新的Bar类型
## Provider && ProviderManager
## 持久化
## Message && MessageManager
## Group && GroupManager
## Portfolio && PortfolioManager
## Report && ReportManager
## RiskManager
## StatisticsManager
 * 提供virtual接口，可以扩展新的统计方式
## Strategy && StrategyManager
 * 提供Strategy接口，比较复杂
 * StrategyManager
## SubscriptionManager
 * 管理订阅服务关系，主要是为了断线后系统提供自动订阅功能
## UserManager
 * 管理用户的配置
## Config && Manager
## Indicator && IndicatorManager
 * 提供Indicator接口，让用户可以通过Script或者Native来扩展
 * 管理Indicator之间的依赖顺序(也许不需要)
 *


<code>
AccountDataManager.cs
DataFileManager.cs
DataManager.cs
EventLoggerManager.cs
EventManager.cs
FileManager.cs
GroupManager.cs
InstrumentManager.cs
OrderManager.cs
OutputManager.cs
PortfolioManager.cs
ProviderManager.cs
ReportManager.cs
RiskManager.cs
ScenarioManager.cs
StatisticsManager.cs
StrategyManager.cs
StreamerManager.cs
SubscriptionManager.cs
UserManager.cs
XmlProviderManagerSettings.cs
</code>

# 特别
## 股票的复权处理
 * 采用过滤器模式

## Forex

## Option

# 系统架构

# 插件架构和接口
