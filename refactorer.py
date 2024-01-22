import os
import re

def search_files(directory, target_directory):
    # Get the list of all directories in the specified directory
    all_directories = [d for d in os.listdir(directory) if os.path.isdir(os.path.join(directory, d))]

    # Check if the target directory is in the list
    if target_directory not in all_directories:
        print(f"Target directory '{target_directory}' not found.")
        exit()

    # Get into the target directory
    target_path = os.path.join(directory, target_directory) 
    os.chdir(target_path)

    # Traverse all directories and find files with .h extension
    file_count = 0
    for root, dirs, files in os.walk(target_path):
        for file in files:
            if file.endswith(".h"):
                print(os.path.join(root, file))
                file_contents = str()
                with open(os.path.join(root, file), 'r') as f:
                    file_contents = f.read()
                updated_content  =re.sub(r'__(.*?)__', r'_'+ file.strip('.h').upper() + '\1_H_', file_contents)
                with open(os.path.join(root, file), 'w') as f:
                    f.write(updated_content)

if __name__ == "__main__":
    # Get the parent directory of the script
    script_directory = os.path.dirname(os.path.abspath(__file__))

    # Get the target directory from the user
    user_input = 'ezlopi-services'
    target_directory = user_input.strip()

    # Perform the search
    search_files(script_directory, target_directory)
