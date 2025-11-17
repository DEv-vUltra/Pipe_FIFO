<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="UTF-8">
<title>README - Pipe & FIFO</title>
<style>
  body { font-family: Arial, sans-serif; line-height: 1.6; padding: 20px; }
  h1, h2, h3 { color: #2c3e50; }
  img { margin: 10px 0; border: 1px solid #ccc; }
  pre { background: #f4f4f4; padding: 10px; border-radius: 5px; }
</style>
</head>
<body>

<h1>Introduction to Pipe_FIFO</h1>

<h2>1.Giới thiệu tổng quan</h2>
<p>Đối với các tiến trình chạy hai chương trình khác nhau, thì có một bài toán được đặt ra là đầu ra của process này có thể là đầu vào của process khác hay không ? Điều này dẫn tới khái niệm về Pipe.<br>
Pipe có thể hiểu đơn giản giống như một đường ống nhưng để là để truyền dữ liệu từ tiến trình này sang tiến trình khác.</p>

<h2>2.Khái niệm và các đặc tính của Pipe</h2>

<h3>a.Pipe là “1 dòng chảy dữ liệu” (byte-stream)</h3>
<p>Điều này nghĩa là dữ liệu sẽ không có khái niệm giống như tin nhắn hay là ranh giới tin nhắn (message boundary) và nó sẽ liên tục như 1 dòng chảy giống như khái niệm của stream socket.<br>
Với đặc tính này thì ta thường phải thiết kế bộ phân tách dữ liệu.</p>

<h3>b.Pipe có tính 1 chiều</h3>
<p>Dữ liệu sẽ chỉ truyền đi 1 chiều chứ không theo chiều ngược lại, 1 đầu dùng để đọc, đầu còn lại dùng để ghi<br>
Tính nguyên tử của Pipe: nếu nhiều tiến trình cùng ghi vào một pipe thì phải có một cơ chế đảm bảo rằng dữ liệu sẽ không xen kẽ với nhau.<br>
Khi mà nhiều tiến trình cùng ghi vào một pipe thì kernel việc 1 block lớn được truyền đi sau đó các nó sẽ được tách ra thành những segment nhỏ hơn để ghi và đôi khi sẽ bị chia ra nhỏ hơn “nguyên tử” mà ta quy định là PIPE_BUF.</p>

<h3>c.Pipe có giới hạn</h3>
<p>Pipe chỉ đơn giản là 1 buffer trong bộ nhớ của kernel và khi đầy thì không thể ghi được nữa trừ khi đầu đọc loại bớt data.</p>

<h2>3.Cách sử dụng Pipes</h2>

<p>Cú pháp để gọi pipe thì là : <code>int pipe(int filedes[2])</code> với filedes[0] là đầu đọc còn filedes[1] là đầu ghi</p>

<img width="1025" height="190" alt="Screenshot from 2025-11-16 16-56-51" src="https://github.com/user-attachments/assets/2ae493fd-013b-495b-a33f-446366a5e2d8" />

<p>Hình ảnh mô phỏng sau khi gọi xong một pipe:</p>

<img width="788" height="242" alt="Screenshot from 2025-11-16 16-59-37" src="https://github.com/user-attachments/assets/6816d1d0-8017-456d-839e-3531625889dc" />

<p>Ta sẽ lấy ví dụ 1 trường hợp cụ thể: dùng pipe giao tiếp giữa process cha và process con.<br>
Khi gọi pipe thì đồng thời gọi fork thì tiến trình con sẽ được thừa kế không gian nhớ của tiến trình cha và ta cũng có thể đóng mở các đầu đọc ghi không cần thiết.</p>

<img width="857" height="494" alt="Screenshot from 2025-11-16 17-07-06" src="https://github.com/user-attachments/assets/dccd80f4-d2f9-475f-a6d2-b2ff3cabbaa0" />

<h2>4.Sự đồng bộ hóa giữa các process</h2>

<p>Trong phần thread và process ta đã được biết đến hiện tượng race condition khi mà các process chúng ghi đè lên nhau. Điều này có thể được khắc phục bằng cách sử dụng 1 đường ống để các process có thể chạy tuần tự.<br>
Ví dụ: ta có 3 tiến trình con và ta muốn nó chạy tuần tự p1 -> p2 -> p3</p>

<p>Ý tưởng:</p>
<ul>
  <li>Tất cả các tiến trình con đọc từ pipe khi chưa được phép chạy → chúng bị block.</li>
  <li>Khi tiến trình cha muốn cho tiến trình đầu tiên chạy, nó ghi một tín hiệu vào pipe.</li>
  <li>Tiến trình nào đọc được tín hiệu sẽ chạy, xong xuôi sẽ ghi tín hiệu lại vào pipe cho tiến trình tiếp theo.</li>
</ul>

<p>Như vậy, pipe hoạt động như một token: chỉ tiến trình có token mới chạy, tạo đồng bộ hóa tuần tự.</p>

<p>Kết quả từ chương trình process_sync.c cho thấy:</p>

<img width="363" height="109" alt="Screenshot from 2025-11-16 17-52-39" src="https://github.com/user-attachments/assets/98bd2303-d524-4778-9463-657b92f87d4a" />

<p>Lưu ý: khi thiết kế chương trình nếu tiến trình con đang chạy thì không đóng đầu ghi và đọc vì nếu làm vậy các tiến trình con tiếp theo sẽ không thể ghi hay đọc được do chúng bị block.</p>

<h2>5.Sử dụng pipe để kết nối các “bộ lọc”</h2>

<p>Trong hệ thống Unix/Linux thì pipe cho phép chuyển đầu ra của 1 chương trình thành đầu vào của 1 chương trình khác. Mỗi chương trình hoạt động như 1 “bộ lọc” như nhận dữ liệu, xử lý và xuất ra.<br>
Trong lập trình nhúng thì mô hình pipe + filter được sử dụng rất phổ biến vì các hệ thống nhúng thường yêu cầu tiết kiệm tài nguyên, xử lý theo dòng, chia nhỏ chức năng và chạy nhiều tiến trình nhỏ thay vì 1 chương trình lớn.</p>

<p>→ Điều này giúp cho việc cách ly lỗi và thay thế trở nên dễ dàng hơn đặc biệt là trong những dự án lớn</p>

<p>Lấy ví dụ cụ thể về việc đọc cảm biến nhiệt độ thì trong thực tế nó được chia ra làm nhiều tiến trình như thế này:<br>
<code>./read_temp | ./kalman_filter | ./save_to_flash</code> (nghĩa là các dữ liệu sẽ được đọc trong tiến trình read_temp sau đó đi qua 1 bộ lọc dữ liệu tên là kalman filter và cuối cùng lưu vào bộ nhớ flash).<br>
Hãy chạy thử chương trình mypipe.c để hiểu rõ hơn. Đây là kết quả:</p>

<img width="377" height="22" alt="Screenshot from 2025-11-17 16-43-29" src="https://github.com/user-attachments/assets/1fc3bafd-834d-46b9-af3a-2facf2954a2e" />

<img width="290" height="105" alt="Screenshot from 2025-11-17 16-43-03" src="https://github.com/user-attachments/assets/7a88e6e7-487d-44c1-a36b-42893fc5d2a4" />

<h2>6.FIFOs</h2>

<p>FIFO cũng giống như pipe nhưng khác với pipe ở chỗ là FIFO có một tên định danh trong hệ thống quản lý file và được mở giống như file thông thường.<br>
Tức là đối với pipe thì không có địa chỉ cụ thể nhưng FIFO thì có và nó được coi là 1 file và điều này giúp cho FIFO là kỹ thuật dùng để kết nối 2 tiến trình không liên quan tới nhau.</p>

<p>Cách sử dụng:</p>

<img width="996" height="199" alt="Screenshot from 2025-11-17 15-49-17" src="https://github.com/user-attachments/assets/4dd69370-300a-48cf-975b-b1b7df0b0644" />

<p>Con trỏ const char *pathname là 1 con trỏ dẫn tới địa chỉ cố định, còn mode ở đây là quyền đọc ghi thực thi ví dụ mode 666, 644, 664 …. <br>
Vì FIFO được coi là 1 file nên bạn phải mở file đó để sử dụng</p>

<pre>FILE *fopen(const char *pathname, const char *mode);</pre>

<p>Mode ở đây có thể là Read hoặc Write, lưu ý ở đây là phải xác định rõ ràng là tiến trình của mình sẽ là đọc hay ghi chứ không nên sử dụng cả read và write (O_RDWR) vì tính chất 1 chiều giống như pipe ta nên xác định đầu nào đọc, đầu nào ghi nếu không thì sẽ không thể xác định được luồng dữ liệu đi như nào và sẽ không gửi tới nếu 1 tiến trình bị kill. Ứng dụng của FIFO có thể giao tiếp giữa các tiến trình không liên quan (tức là tiến trình không phải cha con), cụ thể như giữa server và client.</p>

<p>Lưu ý: nếu dùng một FIFO để gửi phản hồi lại cho nhiều client thì nhiều client sẽ cùng đọc vào FIFO file và dẫn tới race condition nên tách ra thành như này.</p>

<img width="975" height="425" alt="Screenshot from 2025-11-17 16-37-51" src="https://github.com/user-attachments/assets/246de65b-ba13-4459-93bb-da85d4cfbd55" />

<p>Ngoài ra, trong quá trình gửi dữ liệu, ta cũng phải thêm vào 1 bộ phân tách dữ liệu để phân chia dữ liệu vì đặc tính của FIFO cũng giống như Pipe là 1 “dòng chảy dữ liệu” mà không có “sự phân cách dữ liệu”</p>

<img width="964" height="323" alt="Screenshot from 2025-11-17 16-41-19" src="https://github.com/user-attachments/assets/2ba06bb8-44ba-4203-920e-ea3bb76fee23" />

<p>Kết quả trả về của 2 chương trình server và client:</p>

<img width="1676" height="355" alt="Screenshot from 2025-11-17 17-01-49" src="https://github.com/user-attachments/assets/b1fc789d-413b-4cdb-8945-f9d46cac483b" />

</body>
</html>





  

