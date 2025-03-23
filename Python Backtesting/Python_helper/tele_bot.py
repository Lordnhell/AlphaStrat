import json
import telegram
# from telegram import Bot

# Load config
with open('config.json') as config_file:
    config = json.load(config_file)

TELEGRAM_BOT_TOKEN = config['TELEGRAM_BOT_TOKEN']
TRADER_CHAT_IDS = config['TRADER_CHAT_IDS']

bot = Bot(token=TELEGRAM_BOT_TOKEN)

def send_alert_to_trader(trader_name, message):
    """
    Sends a Telegram alert to a specific trader based on their name.
    """
    chat_id = TRADER_CHAT_IDS.get(trader_name)
    if chat_id:
        bot.send_message(chat_id=chat_id, text=message)
        print(f"✅ Alert sent to {trader_name}")
    else:
        print(f"❌ Trader '{trader_name}' not found in chat ID mapping.")
