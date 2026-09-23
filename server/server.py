import os, json, socket, threading, csv, shutil
from datetime import datetime
from flask import render_template, Blueprint, request, jsonify, session, url_for, redirect
from flask.logging import default_handler


app = Flask(__name__)

SAVE_DIR    = os.path.dirname(os.path.abspath(__file__))
times   = []
forces  = []
array_size = 10
@app.route("/")
def home():

    
    return render_template('indexq.html')
   
    
@app.route('/data', methods=["GET", "POST"]) # Receive data as a post request
def soil_data():
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    print(timestamp)
    data = request.get_json(force=True)
   
    if data.get("ff") =="getdata":
        times.clear()
        forces.clear()
        csv_name  = os.path.join(SAVE_DIR, f"penetrometer_.csv") # Create CSV file
        data = request.get_json(force=True)
        line = data.get("gg")
        print(line)
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
        g = open("getdata.json", 'w')
        g.write('{"getdata":"0"}')
        g.close
        foldername = timestamp
        os.makedirs(foldername) # Create folder
        shutil.move(csv_name, foldername)
        shutil.move("coordinates.txt", foldername) # Move data and coordinates to folder, each data point gets its own folder
        
        return "b"
    return "aa"

@app.route("/collect", methods=["GET", "POST"])
def collect():
    data = request.get_json(force=True) # get json data from the post request we sent with the phone
    print(data)

    if data.get('collect') == 'getdata':
        h = open("coordinates.txt", 'w') # Write coordinates to text file
        h.write(data.get("location"))
        h.close

    return "a"
