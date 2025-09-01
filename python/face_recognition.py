import face_recognition
import cv2
import numpy as np
import serial
import time
import sqlite3
from datetime import datetime

# UART bağlantısı
serial1 = serial.Serial('COM5', 9600, timeout=1)

# Veritabanı bağlantısı
conn = sqlite3.connect('access_logs.db')
cursor = conn.cursor()

# Tabloyu oluştur (varsa atlar)
cursor.execute('''
    CREATE TABLE IF NOT EXISTS logs (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT,
        timestamp TEXT
    )
''')
conn.commit()

# Girişleri kaydetme fonksiyonu
def log_access(name):
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    cursor.execute('INSERT INTO logs (name, timestamp) VALUES (?, ?)', (name, timestamp))
    conn.commit()
    print(f"[LOGGED] {name} @ {timestamp}")

# Webcam
video_capture = cv2.VideoCapture(0)

# Yüzler
me_image = face_recognition.load_image_file("me.jpg")
me_face_encoding = face_recognition.face_encodings(me_image)[0]

biden_image = face_recognition.load_image_file("biden.jpg")
biden_face_encoding = face_recognition.face_encodings(biden_image)[0]

known_face_encodings = [
    me_face_encoding,
    biden_face_encoding
]
known_face_names = [
    "Ulas Sakin",
    "Joe Biden"
]

face_locations = []
face_encodings = []
face_names = []
process_this_frame = True

# En son tanınan kişi (tekrar tekrar yazmasın diye)
last_recognized = None
last_sent_time = 0


#time.sleep(1)  # UART stabilitesi için ufak bir bekleme
#print("[INFO] Kayıtlqı kullanıcılar gönderiliyor...")
#for name in known_face_names:
    #serial1.write(f"LIST:{name}\n".encode())
    #serial1.flush()
    #print(f"[UART SEND] LIST:{name}")

    #time.sleep(1.5)  # KL25Z'nin işlemesi için kısa gecikme


while True:
    ret, frame = video_capture.read()

    if process_this_frame:
        small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)
        rgb_small_frame = cv2.cvtColor(small_frame, cv2.COLOR_BGR2RGB)

        face_locations = face_recognition.face_locations(rgb_small_frame)
        face_encodings = face_recognition.face_encodings(rgb_small_frame, face_locations)

        face_names = []
        for face_encoding in face_encodings:
            matches = face_recognition.compare_faces(known_face_encodings, face_encoding)
            name = "Unknown"

            face_distances = face_recognition.face_distance(known_face_encodings, face_encoding)
            best_match_index = np.argmin(face_distances)
            if matches[best_match_index]:
                name = known_face_names[best_match_index]

            face_names.append(name)

            # Yeni bir kullanıcı tanındıysa logla ve gönder
            current_time = time.time()
            if name != "Unknown" and (last_recognized != name or current_time - last_sent_time > 10):
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                serial1.write(f"{timestamp}\n".encode())
                
                print(f"[UART SEND] {timestamp}")

                log_access(name)
                last_recognized = name
                last_sent_time = current_time
            elif name == "Unknown":
                serial1.write(b'0')

    process_this_frame = not process_this_frame

    for (top, right, bottom, left), name in zip(face_locations, face_names):
        top *= 4; right *= 4; bottom *= 4; left *= 4
        cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)
        cv2.rectangle(frame, (left, bottom - 35), (right, bottom), (0, 0, 255), cv2.FILLED)
        font = cv2.FONT_HERSHEY_DUPLEX
        cv2.putText(frame, name, (left + 6, bottom - 6), font, 1.0, (255, 255, 255), 1)

    cv2.imshow('Video', frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        serial1.write(b'0')
        print("System locked.")
        break

serial1.close()
video_capture.release()
cv2.destroyAllWindows()
conn.close()
