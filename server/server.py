import os, json, socket, threading, csv, shutil
from datetime import datetime
from flask import Flask
from flask import render_template, Blueprint, request, jsonify, session, url_for, redirect
from flask.logging import default_handler
import subprocess

app = Flask(__name__)

SAVE_DIR    = os.path.dirname(os.path.abspath(__file__))
times   = []
forces  = []
array_size = 10



@app.route("/", methods=["GET", "POST"])

def home():

    print("ccff")

   
    return render_template('index.html')
    
   
@app.route("/sensor", methods=["GET", "POST"])
def send_sensor_values():
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    global device_count
    times.clear()
    forces.clear()
    csv_name  = os.path.join(SAVE_DIR, f"penetrometer_.csv") # Create CSV file
    request.get_json(force=True)
    data = request.get_json(force=True)
    line = data.get("sense")
    parts = line.split(",")
    print(parts)
    for part in parts[1::2]:
        raw_adc = float(part)
        force_kg = (raw_adc)

        force_zeroed = abs(force_kg)  # abs handles either load cell direction
        forces.append(force_zeroed)
    for time in parts[::2]:
        times.append(time)
    with open(csv_name, "w", newline="") as f: # Write to CSV file
        writer = csv.writer(f)
        writer.writerow(["Time (s)", "Force (kg)"])
        writer.writerows(zip(times, forces))
        print(f"CSV saved   → {csv_name}\n")
        f.close
    with open("coordinates.txt", 'w') as g: # Write coordinates to text file
        g.write(json.dumps(data.get("location")))
        g.close
    foldername = json.dumps(data.get("tip")).strip('"') + " " + timestamp 
    os.makedirs(foldername) # Create folder
    shutil.move(csv_name, foldername)
    shutil.move("coordinates.txt", foldername) # Move data and coordinates to folder, each data point gets its own folder
    return jsonify(data)

