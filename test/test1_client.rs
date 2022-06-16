use std::io::prelude::*;
use std::net::TcpStream;
use std::thread::sleep;
use std::time::Duration;
/*测试buffer用*/
fn main() -> std::io::Result<()> {
    let  mut string_to_write:String = String::from("hello world!\n");
    let mut stream = TcpStream::connect("192.168.238.128:7788")?;
    for _count in 0..20 {
        stream.write(string_to_write.as_bytes())?;
        sleep(Duration::from_secs(1));

        string_to_write = string_to_write.repeat(2);
    }
    string_to_write = String::from("hello world!\n");
    for _count in 0..10 {
        stream.write(string_to_write.as_bytes())?;
        sleep(Duration::from_secs(1));

        string_to_write = string_to_write.repeat(2);
    }
    stream.write(string_to_write.as_bytes())?;
        sleep(Duration::from_secs(1));
    stream.write(string_to_write.as_bytes())?;
        sleep(Duration::from_secs(1));
    stream.write(string_to_write.as_bytes())?;
        sleep(Duration::from_secs(1));
    stream.write(string_to_write.as_bytes())?;
        sleep(Duration::from_secs(1));
    stream.write(string_to_write.as_bytes())?;
        sleep(Duration::from_secs(1));
    stream.shutdown(std::net::Shutdown::Both).unwrap();
    Ok(())
} // the stream is closed here