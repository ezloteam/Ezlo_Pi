import os

# Specify the path where the directories are located
path = './ezlopi-hal'

# List all directories in the specified path
directories = os.listdir(path)

# Iterate through each directory and rename it
for directory in directories:
    # Check if it's a directory and has 'ezlopi_' prefix
    if os.path.isdir(os.path.join(path, directory)) and 'ezlopi-' in directory:
        # Formulate new name by replacing 'ezlopi_' with 'ezlopi-core-'
        # new_name = directory.replace('ezlopi_', 'ezlopi-core-')
        
        # Rename the directory
        # os.rename(os.path.join(path, directory), os.path.join(path, new_name))

        # Now, rename the files within the directory
        for filename in os.listdir(os.path.join(path, directory)):
            if 'ezlopi' in filename:
                new_file_name = filename.replace('ezlopi_core_', 'ezlopi_hal_')
                os.rename(os.path.join(path, directory, filename), os.path.join(path, directory, new_file_name))

print("Directories and files renamed successfully!")
