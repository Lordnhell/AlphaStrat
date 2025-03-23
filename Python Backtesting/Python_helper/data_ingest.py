# data_ingest.py
import os
import pandas as pd

def load_csv_files(directory_path):
    """
    Load all CSV files in the directory into a list of dictionaries:
    [{'filename': <filename>, 'data': <DataFrame>}]
    """
    dataset_list = []

    if not os.path.exists(directory_path):
        raise FileNotFoundError(f"Directory {directory_path} does not exist.")

    csv_files = [file for file in os.listdir(directory_path) if file.endswith('.csv')]

    if not csv_files:
        print("No CSV files found in the directory.")
        return dataset_list

    for csv_file in csv_files:
        file_path = os.path.join(directory_path, csv_file)
        try:
            df = pd.read_csv(file_path)
            print(f"\n Loaded: {csv_file}")
            print(df.head())
            dataset_list.append({'filename': csv_file, 'data': df})
        except Exception as e:
            print(f" Error loading {csv_file}: {e}")

    return dataset_list
