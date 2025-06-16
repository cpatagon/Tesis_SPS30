import subprocess, os, sys

def build_and_run():
    compile_cmd = [
        'gcc', '-I', 'Tests/stubs', '-I', 'APIs/Inc', '-I', 'APIs/Config',
        'Tests/data_logger_buffers_runner.c',
        'Tests/stubs/time_rtc.c', 'Tests/stubs/microSD_utils.c',
        'Tests/stubs/fatfs_stub.c', '-o', 'Tests/data_logger_buffers_runner'
    ]
    subprocess.check_call(compile_cmd)
    result = subprocess.run(['Tests/data_logger_buffers_runner'], capture_output=True, text=True)
    return result

def test_store_measurement():
    result = build_and_run()
    assert result.returncode == 0, result.stdout + result.stderr
    assert 'PASS' in result.stdout
