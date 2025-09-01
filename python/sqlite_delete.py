import sqlite3

conn = sqlite3.connect('access_logs.db')
cursor = conn.cursor()

# Tüm kayıtları sil
cursor.execute("DELETE FROM logs")
conn.commit()

print("All entries are deleted.")
conn.close()
