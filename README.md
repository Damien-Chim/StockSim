# StockSim

Link: https://stock-sim-self-five.vercel.app/

StockSim is a full-stack stock exchange simulator built around a C++ matching engine. It simulates a simple electronic market in which users and automated trading bots can place limit buy and sell orders, with trades executed using price-time priority.

The project includes a C++ backend, HTTP API, React frontend, automated market participants, candlestick data, portfolio tracking, and a web deployment using Docker, Render, and Vercel.

## Features

- Limit buy and sell orders
- Price-time priority order matching
- Partial order fills
- Order cancellation
- Cash and stock reservation for open orders
- User portfolio and balance tracking
- Automated trading bots
- Simulated live market activity
- Trade history
- Candlestick/OHLCV data
- Best bid and ask display
- Order history and status tracking
- Simulation reset
- REST-style HTTP API
- Web-based trading interface

## How It Works

StockSim models a simplified electronic stock exchange.

Each stock maintains an order book containing buy and sell orders at different price levels. When a new order is submitted, the matching engine attempts to match it against compatible orders on the opposite side of the book.

Buy orders are prioritised by:

1. Higher price
2. Earlier arrival time

Sell orders are prioritised by:

1. Lower price
2. Earlier arrival time

A trade can occur when:

```text
highest bid >= lowest ask
```

Orders may be completely or partially filled. Any remaining quantity stays active in the order book.

## Architecture

```text
┌─────────────────────────────┐
│       React + Vite UI       │
│                             │
│ Charts, Orders, Portfolio   │
└──────────────┬──────────────┘
               │
               │ HTTP / JSON
               ▼
┌─────────────────────────────┐
│     C++ HTTP Server         │
│    Boost.Asio / Beast       │
└──────────────┬──────────────┘
               │
               ▼
┌─────────────────────────────┐
│          Exchange           │
│                             │
│ Users · Stocks · Orders     │
└──────────────┬──────────────┘
               │
       ┌───────┴────────┐
       ▼                ▼
┌─────────────┐   ┌─────────────┐
│ Order Books │   │ Bot Manager │
│             │   │             │
│ Buy / Sell  │   │ Auto Trades │
│ Matching    │   │             │
└──────┬──────┘   └─────────────┘
       │
       ▼
┌─────────────────────────────┐
│           Trades            │
│                             │
│ Price · Quantity · Time     │
│ Candlestick Generation      │
└─────────────────────────────┘
```

### Backend

The backend is written in C++ and contains the core simulation logic.

The main components include:

- **Exchange** — manages users, stocks and orders.
- **Stock** — represents a listed stock and maintains its market data and order book.
- **OrderBook** — maintains buy and sell price levels and performs order matching.
- **Order** — represents a limit buy or sell order.
- **Trade** — represents an executed transaction.
- **User** — tracks cash, holdings, reserved assets and orders.
- **BotManager** — creates automated traders that continuously submit orders.
- **HttpServer** — exposes the simulation through an HTTP/JSON API.

Shared exchange state is synchronised to allow the HTTP server and trading bot thread to interact with the market concurrently.

### Frontend

The frontend is built using React, TypeScript and Vite.

It communicates with the C++ backend through HTTP requests and provides an interface for:

- viewing available stocks
- viewing current market prices
- viewing candlestick charts
- viewing recent trades
- placing buy and sell orders
- cancelling active orders
- viewing portfolio holdings and cash
- viewing order status
- resetting the simulation

## Order Matching

StockSim uses price-time priority.

For example, suppose the sell side contains:

```text
SELL 5 @ $100
SELL 5 @ $102
```

and a new order arrives:

```text
BUY 8 @ $102
```

The buy order can match both price levels.

The result is:

```text
5 shares matched @ $100
3 shares matched @ $102
```

The second sell order remains in the book with:

```text
SELL 2 @ $102
```

The matching engine also handles partial fills and updates the corresponding users, orders, balances, holdings and market data.

## Asset Reservation

StockSim distinguishes between available and reserved assets.

For a buy order:

```text
available cash → reserved cash
```

For a sell order:

```text
available stocks → reserved stocks
```

This prevents a user from placing multiple orders using the same cash or shares.

When an order executes, the appropriate reserved assets are consumed. If an order is cancelled, its remaining reserved assets are returned to the user's available balance or holdings.

## Automated Trading

StockSim includes automated traders to keep the simulated market active.

Each bot is given cash and stock holdings and continuously generates small buy and sell orders around the current market price.

This produces:

- changing market prices
- bid/ask activity
- trade history
- candlestick data

without requiring the user to manually provide both sides of every trade.

## Candlestick Data

Executed trades are aggregated into candlesticks containing:

```text
Open
High
Low
Close
Volume
```

These candles are exposed through the HTTP API and displayed by the frontend as a market price chart.

## API

The C++ backend exposes HTTP endpoints used by the frontend.

### Market Data

```text
GET /api/stocks
GET /api/stocks/:stock_id/basic_info
GET /api/stocks/:stock_id/candles
GET /api/stocks/:stock_id/trades
```

### User

```text
GET /api/me
GET /api/me/orders
```

### Trading

```text
POST /api/orders/buy
POST /api/orders/sell
POST /api/orders/cancel
```

Example buy request:

```json
{
  "stock_id": "STOCK_0",
  "quantity": 5,
  "limit_price": 105
}
```

### Simulation

```text
POST /api/reset
```

Resetting stops the trading bots, clears the current exchange state, restores the initial stocks and user account, and restarts the bots.

## Technology Stack

### Backend

- C++20
- Boost.Asio
- Boost.Beast
- nlohmann/json
- CMake
- vcpkg

### Frontend

- React
- TypeScript
- Vite

### Deployment

- Docker
- Render — backend
- Vercel — frontend

## Running Locally

### Prerequisites

You will need:

- a C++20-compatible compiler
- CMake
- vcpkg
- Node.js / npm

### Backend

Clone the repository:

```bash
git clone <repository-url>
cd StockSim
```

Configure the project with CMake and your vcpkg toolchain:

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

Build:

```bash
cmake --build build
```

Run the HTTP server:

```bash
./build/StockSim 1
```

On Windows, depending on the CMake generator:

```powershell
.\build\Debug\StockSim.exe 1
```

The server uses the `PORT` environment variable when provided. For example, in PowerShell:

```powershell
$env:PORT="9000"
```

The deployed backend uses this so that the hosting platform can choose the port on which the server listens.

### Frontend

Navigate to the frontend directory:

```bash
cd frontend
```

Install dependencies:

```bash
npm install
```

Create a local environment variable pointing to the backend:

```text
VITE_API_URL=http://127.0.0.1:8080
```

Then run:

```bash
npm run dev
```

## Environment Variables

### Backend

```text
PORT
CORS_ORIGIN
```

`PORT` determines the HTTP server's listening port.

`CORS_ORIGIN` determines which frontend origin is allowed to make browser requests to the backend.

Example:

```text
CORS_ORIGIN=http://localhost:5173
```

### Frontend

```text
VITE_API_URL
```

Example:

```text
VITE_API_URL=http://127.0.0.1:8080
```

For production, this should point to the deployed backend instead.

## Docker

The backend can also be built and run inside Docker.

Build the image from the repository root:

```bash
docker build -t stocksim .
```

Run it:

```bash
docker run -p 8080:8080 stocksim
```

Environment variables can be supplied when starting the container if required.

## Current Limitations

StockSim is intended as an educational simulation rather than a production trading system.

Currently:

- there is one shared simulation running on the backend
- the frontend uses a single predefined user
- there is no authentication or persistent user account system
- simulation state is stored in memory
- restarting the backend resets the simulation
- all visitors interact with the same deployed market
- the simulation uses simplified execution and market behaviour

These are deliberate simplifications that keep the project focused on exchange mechanics, order matching, concurrency, networking and full-stack integration.

## Possible Future Improvements

Potential extensions include:

- multiple user accounts and authentication
- isolated simulation sessions
- persistent storage
- market orders
- additional order types
- configurable trading bots
- improved market-making strategies
- order book depth visualisation
- WebSocket-based real-time updates
- configurable simulation speed
- historical market statistics

## Project Motivation

StockSim started as a C++ project for exploring how an electronic exchange could be represented using object-oriented design and data structures.

It gradually expanded from a local matching engine into a full-stack application involving concurrency, automated traders, HTTP networking, JSON APIs, a React frontend, containerisation and cloud deployment.

The project is not intended to reproduce every detail of a real financial exchange. Instead, it focuses on implementing the core mechanics behind order submission, price-time priority, matching, execution and portfolio updates in a system that can be interacted with through a web interface.
