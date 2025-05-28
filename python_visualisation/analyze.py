import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from matplotlib.ticker import MaxNLocator
import matplotlib as mpl
from multiprocessing import Process
from difflib import SequenceMatcher
import os
from sys import argv
from time import sleep
import numpy as np

plt.close("all")

def parse_data_to_days(df): 
    # convert the 'Time' column to datetime, remove localization
    df['Time'] = pd.to_datetime(df['Time']).dt.tz_localize(None)

    # extract the date
    df['Date'] = df['Time'].dt.date
    
    # group by the 'Date' column
    grouped = df.groupby('Date')
    
    # create a dictionary of different days
    daily_data = {date: group.drop(columns=['Date']) for date, group in grouped}
    
    return daily_data

def parse_data_to_hours(data):
    # extract hour from timestamp
    data['Hour'] = data['Time'].dt.hour  

    # group data by hour for the boxplot
    hourly_data = [data[data['Hour'] == hour]for hour in range(24)]

    return hourly_data

# Save plot into file
def export_figure(plt, filename):
    # use format SVG / EPS / PDF
    ext = ".eps"

    # get complete path
    file_path = os.path.join(output_path, filename)
    
    # save figure
    plt.savefig(file_path+ext, format=ext[1:])

    # process the PDF to remove identical objects
    if ext == ".eps":
        sleep(4)
        os.system(f"epstopdf {file_path+ext} {file_path+".pdf"}")
        try:
            os.remove(file_path+ext)
        except FileNotFoundError:
            pass

    ext = ".png"

    # save the figure
    plt.savefig(file_path+ext, format=ext[1:])

    print(f"Saved figure to {file_path}")

# get longest common substring from two strings
def common_substring(str1, str2):
    name_index = SequenceMatcher(None, str1,str2).find_longest_match()
    return str1[name_index.a:name_index.a + name_index.size]

# Plot RSSI in time, each day in a separate boxplot
def plot_daily(daily_data, columns_to_plot):
    global tick_size
    global label_size
    
    y_name = common_substring(columns_to_plot[0], columns_to_plot[1])
    for i,name in enumerate(columns_to_plot):
        plot_data = []
        dates = []
        
        for date, data in daily_data.items():
            # append the data for the selected column and corresponding date
            plot_data.append(data[name].values)
            dates.append(date)
        
        plt.subplot(1,2,i+1)
        plt.boxplot(plot_data, tick_labels=dates, showmeans=False, notch=False, sym='.')
        
        #plt.title(f"{name}")
        plt.xlabel("Date", fontsize=label_size+5)
        plt.ylabel(columns_to_plot[i], fontsize=label_size+5)
        plt.xticks(rotation=25, fontsize=tick_size)
        
        ax = plt.gca()
        ax.yaxis.set_major_locator(MaxNLocator(integer=True))  # set integer ticks
        ax.tick_params(axis='y', labelsize=tick_size)

    
    plt.subplots_adjust(
        bottom=0.15,
        wspace=0.25,
        right=0.97,
        left=0.1,
        top=0.96
    )

    # save figure into file and show figure
    out_filename = y_name.replace(" [dBm]","").replace(" [dB]","").replace(" ","_")
    if out_filename[0] == "_": out_filename = out_filename[1:]
    export_figure(plt, out_filename + "_daily")
    if render: plt.show()


# plot raw data and 15 minute running average
def plot_raw_and_avg(daily_data, column_to_plot, start_day=0):
    global tick_size
    global label_size

    num_days = len(daily_data)
          
    if num_days == 1:
        # handle single-day case without subplots, but match figure size
        date, data = next(iter(daily_data.items()))
        data = data.set_index('Time')
        data['15min_avg'] = data[column_to_plot].rolling('15min').mean()

        fig, ax = plt.subplots(1, 1, figsize=(15, 10))  # same height as multi-day plot

        ax.plot(data.index, data[column_to_plot], label='Raw values', color='blue', alpha=0.7, linewidth=0.2)
        ax.plot(data.index, data['15min_avg'], label='15-Minute average', color='red', linewidth=2)

        ax.legend(loc="lower left", prop={'size': 15})
        ax.set_xlabel("Time", fontsize=label_size+5)
        ax.set_ylabel(column_to_plot, fontsize=label_size+5)
        ax.tick_params(axis='y', labelsize=tick_size+3)
        ax.tick_params(axis='x', labelsize=tick_size+3)

        ax.xaxis.set_major_locator(mdates.HourLocator(interval=2))
        ax.xaxis.set_major_formatter(plt.matplotlib.dates.DateFormatter('%H:%M'))

        filename = column_to_plot.replace(" [dBm]","").replace(" [dB]","").replace(" ","_")
        export_figure(plt, f"{filename}_raw_avg")
        if render: plt.show()
        return


    # create figure with one column and 4 days
    fig, axs = plt.subplots(4, 1, sharex=False, gridspec_kw={'hspace': 0.25,'top':0.95,'bottom':0.1})

    # if there's only one day, axs will not be an array, so wrap it in a list for consistent indexing
    if num_days == 1:
        axs = [axs]
    
    for idx, (date, data) in enumerate(daily_data.items()):
        # first showed day, used to show bot weekday and weekend days in one plot
        day_start = 1
        if idx < day_start:
            continue

        if idx == day_start + 4:
            break

        idx -= day_start
    
        # set the Time column as the index for easier resampling
        data = data.set_index('Time')
        
        # 15-minute moving average
        data['15min_avg'] = data[column_to_plot].rolling('15min').mean()
        
        # plot
        axs[idx].plot(data.index, data[column_to_plot], label='Raw values', color='blue', alpha=0.7, linewidth=0.2)
        axs[idx].plot(data.index, data['15min_avg'], label='15-Minute average', color='red', linewidth=2)

        #axs[idx].set_ylabel(column_to_plot, fontsize=label_size-4)
        axs[idx].legend(loc="lower left", prop={'size': 15})
        
        # x-axis 24-hour
        start_of_day = pd.Timestamp(date)
        end_of_day = start_of_day + pd.Timedelta(days=1)

        axs[idx].set_xlim(start_of_day, end_of_day)
        axs[idx].xaxis.set_major_locator(mdates.HourLocator(interval=2))
        axs[idx].xaxis.set_major_formatter(plt.matplotlib.dates.DateFormatter('%H:%M'))

        # tick sizes
        axs[idx].tick_params(axis='y', labelsize=tick_size+3)
        axs[idx].tick_params(axis='x', labelsize=tick_size+3)


    # add time x label on last plot
    axs[-2].set_ylabel(column_to_plot, fontsize=label_size+5)
    axs[-2].yaxis.set_label_coords(-.07, 1.2)
    axs[-1].set_xlabel("Time", fontsize=label_size+5)

    # save figure into file and show figure
    filename = column_to_plot.replace(" [dBm]","").replace(" [dB]","").replace(" ","_")
    export_figure(plt, f"{filename}_raw_avg")
    if render: plt.show()

# plot histogram of selected column
def plot_histogram(data, column_to_plot):
    global tick_size
    global label_size

    # concatenate RSSI values for single plot
    combined_rssi = data[column_to_plot] #pd.concat([data[columns_to_plot[0]], data[columns_to_plot[1]]])
    
    # longest shared substring
    #y_name = common_substring(columns_to_plot[0], columns_to_plot[1])

    plt.figure()
    bins = plt.hist(combined_rssi,bins=range(int(combined_rssi.min()), int(combined_rssi.max()+2),1), color='skyblue', edgecolor='black', zorder=2, align="left")
    plt.yscale('log')
    plt.grid(True, which="both", zorder=1)

    #plt.title(f'Histogram of combined local {y_name.split()[0]} and peer {y_name.split()[0]}')
    plt.xlabel(column_to_plot, fontsize=label_size)
    plt.ylabel('Count [-]', fontsize=label_size)
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.yticks(fontsize=tick_size)
    plt.xticks(fontsize=tick_size)
    
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))

    # save figure
    export_figure(plt, f"{column_to_plot.split()[0]}_histogram")
    if render: plt.show()


def plot_dropped_packets(data):
    global tick_size
    global label_size

    # calculate number of dropped packets
    data['Dropped Packets Local-Peer'] = (data['Local Tx Count'] - data['Peer Rx Count']).clip(lower=0)
    data['Dropped Packets Peer-Local'] = (data['Peer Tx Count'] - data['Local Rx Count']).clip(lower=0)

    # check if all data is from the same day
    single_day = data['Time'].dt.normalize().nunique() == 1

    time_format = mdates.DateFormatter('%H:%M') if single_day else mdates.DateFormatter('%Y-%m-%d %H:%M')

    plt.subplot(2,1,1)
    plt.plot(data['Time'], data['Dropped Packets Local-Peer'], color='purple', label='Dropped Packets', linewidth=0.8)
    plt.xlabel('Time', fontsize=label_size+5)
    plt.ylabel('Dropped Packets Downlink', fontsize=label_size)
    plt.grid(True, which='both')
    plt.xticks(rotation=10, fontsize=tick_size)
    plt.yticks(fontsize=tick_size)
    plt.gca().xaxis.set_major_formatter(time_format)

    plt.subplot(2,1,2)
    plt.plot(data['Time'], data['Dropped Packets Peer-Local'], color='purple', label='Dropped Packets', linewidth=0.8)
    plt.xlabel('Time', fontsize=label_size+5)
    plt.ylabel('Dropped Packets Uplink', fontsize=label_size)
    plt.grid(True, which='both')
    plt.xticks(rotation=10, fontsize=tick_size)
    plt.yticks(fontsize=tick_size)
    plt.gca().xaxis.set_major_formatter(time_format)

    plt.subplots_adjust(
        top=0.93, bottom=0.12, left=0.1, hspace=0.4
    )

    export_figure(plt, "Dropped_packets")
    if render: plt.show()


# plot hourly boxplots of selcted column
def plot_boxplot_hourly(data, columns_to_plot):
    global tick_size
    global label_size

    data['Time'] = pd.to_datetime(data['Time'])
    data['Hour'] = data['Time'].dt.hour
    
    # longest shared substring
    y_name = common_substring(columns_to_plot[0], columns_to_plot[1])    

    for i,name in enumerate(columns_to_plot):
        # group by hour
        hourly_data = [data[data['Hour'] == hour][name] for hour in range(24)]

        # find longest shared substring
        #name = common_substring(columns_to_plot[0], columns_to_plot[1])

        plt.subplot(2,1,i+1)
        plt.boxplot(hourly_data, tick_labels=[f"{hour}:00" for hour in range(24)], showmeans=False, notch=False, sym='.')

        #plt.title(f"{columns_to_plot[i]} values by hour")
        plt.xlabel("Hour of the day", fontsize=label_size)
        plt.ylabel(columns_to_plot[i], fontsize=label_size)
        plt.xticks(rotation=25, fontsize=tick_size)
        plt.yticks(fontsize=tick_size)
        plt.grid(axis='y')

    plt.subplots_adjust(
        top=0.94, bottom=0.11, left=0.1, hspace=0.3
    )

    export_figure(plt, f"{y_name.split()[0]}_hourly")
    if render: plt.show()


def plot_dropped_packets_per_day(daily_data, column_tx, column_rx, log_threshold=100):
    global tick_size
    global label_size

    dates = []
    downlink_drops = []
    uplink_drops = []

    for date, data in daily_data.items():
        dates.append(pd.to_datetime(date))

        # downlink
        start_tx_down = data[column_tx[0]].iloc[0]
        start_rx_down = data[column_rx[0]].iloc[0]
        end_tx_down   = data[column_tx[0]].iloc[-1]
        end_rx_down   = data[column_rx[0]].iloc[-1]
        dropped_down  = max(0, end_tx_down - end_rx_down) - max(0, start_tx_down - start_rx_down)
        downlink_drops.append(dropped_down)

        # uplink
        start_tx_up = data[column_tx[1]].iloc[0]
        start_rx_up = data[column_rx[1]].iloc[0]
        end_tx_up   = data[column_tx[1]].iloc[-1]
        end_rx_up   = data[column_rx[1]].iloc[-1]
        dropped_up  = max(0, end_tx_up - end_rx_up) - max(0, start_tx_up - start_rx_up)
        uplink_drops.append(dropped_up)

    # bar width and positioning
    bar_width = 0.4
    x = np.arange(len(dates))

    fig, ax = plt.subplots(figsize=(15, 10))

    # downlink and uplink
    ax.bar(x - bar_width / 2, downlink_drops, width=bar_width, label="Downlink", color="green", edgecolor="black", alpha=0.7)
    ax.bar(x + bar_width / 2, uplink_drops, width=bar_width, label="Uplink", color="blue", edgecolor="black", alpha=0.7)

    # y-axis scale
    if max(downlink_drops + uplink_drops) > log_threshold:
        ax.set_yscale("log")
    else:
        ax.set_yscale("linear")

    # x-axis formatting
    ax.set_xticks(x)
    ax.set_xticklabels([d.strftime('%Y-%m-%d') for d in dates], rotation=15, fontsize=tick_size)

    ax.set_xlabel("Date", fontsize=label_size+10)
    ax.set_ylabel("Dropped Packets", fontsize=label_size+10)
    ax.tick_params(axis='y', labelsize=tick_size)
    ax.grid(axis="y", linestyle="--", alpha=0.7)
    ax.legend(fontsize=label_size)

    plt.subplots_adjust(
        top=0.95, bottom=0.15, right=0.95
    )

    export_figure(plt, "dropped_packets_per_day")
    if render: plt.show()


# Input filename
file_path = argv[1]

if len(argv) < 2:
    argv.append(0)

# show figures? used for automated processing of multiple measurements
render = True

# check if input file exists
if not os.path.isfile(file_path):
    print("Invalid file path!")
    exit()

# output folder
output_path = os.path.join("./figures", file_path.split("/")[-1].replace(".csv", ""))

# check if output directory exists
if not os.path.isdir(output_path):
    os.makedirs(output_path)

# clear output folder
os.system(f"rm {os.path.join(output_path,"*")}")

# parse data
print("Parsing data...")
try:
    data = pd.read_csv(file_path, comment='#')
    data.dropna(how="all", inplace=True) 

    # filter out out of bounds values that dont make sense
    data = data[(data["Local RSSI [dBm]"] >= -150) & (data["Local RSSI [dBm]"] <= -30)]
    data = data[(data["Peer RSSI [dBm]"] >= -150) & (data["Peer RSSI [dBm]"] <= -30)]
    data = data[(data["Local SNR [dB]"] >= -20) & (data["Local SNR [dB]"] <= 20)]
    data = data[(data["Peer SNR [dB]"] >= -20) & (data["Peer SNR [dB]"] <= 20)]

except pd.errors.ParserError:
    print("Error parsing file!")
    exit()

# group data into days
daily_data = parse_data_to_days(data)

# set all figures to bigger size
mpl.rcParams['figure.figsize'] = [15, 10]
mpl.rcParams['toolbar'] = 'None'

tick_size = 19
label_size = 25

# calculate Packet error rates
numeric_cols = ['Local Tx Count', 'Local Rx Count', 'Peer Tx Count', 'Peer Rx Count']
last_values = data.tail(1)[numeric_cols]
print(last_values)
print()

downlink_PER = (1 - int(last_values.iloc[0,3])/int(last_values.iloc[0,0])) * 100 # [%]  [Peer RX / Local Tx]
uplink_PER = (1 - int(last_values.iloc[0,1])/int(last_values.iloc[0,2])) * 100 # [%]    [Local RX / Peer Tx]

print(f"Calculated packet error rates:\n    Downlink PER:\t{downlink_PER} %\n    Uplink PER:\t\t{uplink_PER} %")

# qrite PER to file
with open(f"{os.path.join(output_path, "stats.txt")}","w") as out:
    out.write(last_values.__str__())
    out.write("\n\n")
    out.write(f"Calculated packet error rates:\n    Downlink PER:\t{downlink_PER} %\n    Uplink PER:\t\t{uplink_PER} %")
    print("PER saved!")

# all plots to show
plot_functions = [
        (plot_daily, daily_data, ['Peer RSSI [dBm]', 'Local RSSI [dBm]']),
        (plot_daily, daily_data, ['Peer SNR [dB]', 'Local SNR [dB]']),
        (plot_raw_and_avg, daily_data, 'Peer RSSI [dBm]', argv[2]),
        #(plot_raw_and_avg, daily_data, 'Local RSSI [dBm]'),
        #(plot_boxplot_hourly, data, ['Peer RSSI [dBm]', 'Local RSSI [dBm]']),
        #(plot_boxplot_hourly, data, ['Peer SNR [dB]', 'Local SNR [dB]']),
        #(plot_histogram,data, 'Local RSSI [dBm]'),
        #(plot_histogram,data, 'Local SNR [dB]'),
        (plot_dropped_packets, data),
        (plot_dropped_packets_per_day, daily_data, ['Local Tx Count', 'Peer Tx Count'], ['Peer Rx Count', 'Local Rx Count'])
    ]

# all figures are separate processes, this increases memory usage but is much faster
print("Plotting data...")
processes = []
for func, *args in plot_functions:
    p = Process(target=func, args=args)
    p.start()
    processes.append(p)


# wait for all processes to complete
for p in processes:
    p.join()

