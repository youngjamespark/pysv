# import librosa
from pydub import AudioSegment
import pysv
import os
# from pathlib import path

def mp3towav(mp3_file, wav_file):
    mp3_tmp_audio_data = AudioSegment.from_mp3(mp3_file)
    mp3_tmp_audio_data.export(wav_file, format="wav")


# mp3towav("rt1001_005.mp3", "rt1001_005.wav")
file = ["wave\\rt701_005.mp3", "wave\\rt701_007.mp3", "wave\\rt701_012.mp3", "wave\\rt701_014.mp3",
        "wave\\rt701_019.mp3"]

print("current working directory = ", os.getcwd())

for i in range(len(file)):
    sound = AudioSegment.from_mp3(file[i])
    print(f"in file = {file[i]}, frame rate = {sound.frame_rate}, channel = {sound.channels}")
    tmp_file = file[i].replace(".mp3", ".tmp")
    out_file = file[i].replace(".mp3", ".wav")

    if sound.channels != 1:
        sound = sound.set_channels(1)

    if sound.frame_rate == 16000:
        sound.export(out_file, format="wav")
    else:
        sound.export(tmp_file, format="wav")
        print("before")
        pysv.samplerate_change(tmp_file, out_file, 16000)
        os.remove(tmp_file)

        sound = AudioSegment.from_file(out_file, format='wav')
        print(f"out file = {out_file}, frame rate = {sound.frame_rate}, channel = {sound.channels}")
        print("conversion completed.\n")
