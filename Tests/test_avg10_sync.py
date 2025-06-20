import subprocess

def build_and_run():
    compile_cmd = [
        'gcc','-I','Tests/stubs','-I','APIs/Inc',
        'Tests/avg10_sync_runner.c',
        'Tests/stubs/time_rtc.c','Tests/stubs/microSD_utils.c','Tests/stubs/fatfs_stub.c',
        '-o','Tests/avg10_sync_runner'
    ]
    subprocess.check_call(compile_cmd)
    return subprocess.run(['Tests/avg10_sync_runner'], capture_output=True, text=True)

def test_avg10_flag():
    res = build_and_run()
    assert res.returncode == 0, res.stdout+res.stderr
    assert 'PASS' in res.stdout
