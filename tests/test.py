import pysv
from pysv.pysv import pysv_state

info = pysv_state()
# file = "d:\\Audio_Database\\Reference_norm\\4970-29093-0016.wav"
# fileo = "d:\\Audio_Database\\Reference_norm\\4970-29093-0016_norm.wav"
# info = pysv.calculate(file)
# print("File name = ", file)
# print("Samples = ", info.n)
# print(f"Min = {info.maxN}")
# print(f"Max = {info.maxP}")
# print(f"RMS level = {info.rmsdB:.2f} [dB]")
# print(f"Active speech level = {info.ActiveSpeechLevel:.2f} [dB]")
# print(f"Active = {info.ActivityFactor:.2f} %")

# info = pysv.normalize(file, fileo, -35.0)
# print("File name = ", fileo)
# print("Samples = ", info.n)
# print(f"Min = {info.maxN}")
# print(f"Max = {info.maxP}")
# print(f"RMS level = {info.rmsdB:.2f} [dB]")
# print(f"Active speech level = {info.ActiveSpeechLevel:.2f} [dB]")
# print(f"Active = {info.ActivityFactor:.2f} %")

file = "d:\\Audio_Database\\Reference_norm\\5105-28241-0007.wav"
fileo = "d:\\Audio_Database\\Reference_norm\\5105-28241-0007_norm.wav"
# info = pysv.calculate(file)
# print("File name = ", file)
# print("Samples = ", info.n)
# print(f"Min = {info.maxN}")
# print(f"Max = {info.maxP}")
# print(f"RMS level = {info.rmsdB:.2f} [dB]")
# print(f"Active speech level = {info.ActiveSpeechLevel:.2f} [dB]")
# print(f"Active = {info.ActivityFactor:.2f} %")

pysv.normalize(file, fileo, -26)

fileso = "d:\\Audio_Database\\Reference_norm\\5105-28241-0007_8k.wav"
pysv.samplerate_change(file, fileso, 8000)