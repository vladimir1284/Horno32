import requests
import os
from pathlib import Path

def upload_firmware(md5_file_path, bin_file_path):
    """
    Upload firmware file to the specified endpoint
    
    Args:
        md5_file_path (str): Path to the MD5 file
        bin_file_path (str): Path to the binary firmware file to upload
    """
    
    # Check if files exist
    if not os.path.exists(md5_file_path):
        print(f"Error: MD5 file '{md5_file_path}' not found")
        return False
    
    if not os.path.exists(bin_file_path):
        print(f"Error: Binary file '{bin_file_path}' not found")
        return False
    
    # URL for the upload endpoint
    url = 'http://192.168.88.155/rest/uploadFirmware'
    
    # Headers
    headers = {
        'Accept': '*/*',
        'Accept-Language': 'es-ES,es;q=0.9,en;q=0.8',
        'Authorization': 'Basic',
        'Connection': 'keep-alive',
        'Origin': 'http://192.168.88.155',
        'Referer': 'http://192.168.88.155/system/update',
        'Sec-Fetch-Dest': 'empty',
        'Sec-Fetch-Mode': 'cors',
        'Sec-Fetch-Site': 'same-origin',
        'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/139.0.0.0 Safari/537.36',
        'sec-ch-ua': '"Not;A=Brand";v="99", "Google Chrome";v="139", "Chromium";v="139"',
        'sec-ch-ua-mobile': '?0',
        'sec-ch-ua-platform': '"Linux"'
    }
    
    try:
        # First: Upload MD5 file as-is
        print("Uploading MD5 file...")
        with open(md5_file_path, 'rb') as md5_file:
            md5_filename = os.path.basename(md5_file_path)
            files = {
                'file': (md5_filename, md5_file, 'application/octet-stream')
            }
            
            md5_response = requests.post(url, headers=headers, files=files)
            
            print(f"MD5 Status Code: {md5_response.status_code}")
            print(f"MD5 Response: {md5_response.text}")
            
            if md5_response.status_code != 200:
                print("MD5 upload failed")
                return False
        
        # Second: Upload binary file after MD5 success
        print("Uploading binary file...")
        with open(bin_file_path, 'rb') as bin_file:
            bin_filename = os.path.basename(bin_file_path)
            files = {
                'file': (bin_filename, bin_file, 'application/octet-stream')
            }
            
            bin_response = requests.post(url, headers=headers, files=files)
            
            print(f"Binary Status Code: {bin_response.status_code}")
            print(f"Binary Response: {bin_response.text}")
            
            if bin_response.status_code == 200:
                print("Upload successful!")
                return True
            else:
                print("Binary upload failed")
                return False
                
    except Exception as e:
        print(f"Error during upload: {e}")
        return False

def main():
    # Find the firmware files
    build_dir = Path("build/release")
    
    if not build_dir.exists():
        print("Error: build/release directory not found")
        return
    
    # Find .md5 files
    md5_files = list(build_dir.glob("*.md5"))
    if not md5_files:
        print("No .md5 files found in build/release/")
        return
    
    # Use the first .md5 file
    md5_file = str(md5_files[0])
    
    # Find corresponding .bin file
    bin_filename = md5_files[0].stem + ".bin"
    bin_file = str(build_dir / bin_filename)
    
    if not os.path.exists(bin_file):
        print(f"Error: Binary file '{bin_file}' not found")
        return
    
    print(f"MD5 file: {md5_file}")
    print(f"Binary file: {bin_file}")
    
    # Upload the firmware
    upload_firmware(md5_file, bin_file)

if __name__ == "__main__":
    main()