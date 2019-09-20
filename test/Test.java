import java.sql.Connection;
import java.sql.Date;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.Calendar;

public class Test {

	public static void main(String[] args) {

		// auto close connection
		try (Connection conn = DriverManager.getConnection("jdbc:postgresql://127.0.0.1:5433/test?binaryTransfer=false")) {

			try(Statement stmt = conn.createStatement()) {
				stmt.execute("delete from test;");
			}
			try (PreparedStatement stmt = conn.prepareStatement("insert into test values(?,?,?,?)")) {
				for (int i = 0; i < 1000; ++i) {
					stmt.setInt(1, i);
					stmt.setString(2, "test-" + i);
					stmt.setDouble(3, i * 1.5);
					Calendar cal = Calendar.getInstance();
					cal.set(Calendar.MONTH, i % 12);
					stmt.setDate(4, new java.sql.Date(cal.getTime().getTime()));
					stmt.execute();
				}
			}
			System.out.println("Insert done");
			try (PreparedStatement stmt = conn.prepareStatement("select * from test where a=?")) {
				Calendar inst = Calendar.getInstance();
				for (int i = 0; i < 1000; ++i) {
					stmt.setInt(1, i);
					try (ResultSet rs = stmt.executeQuery()) {
						while (rs.next()) {
							// Retrieve by column name
							int a = rs.getInt("a");
							String b = rs.getString("b");
							float c = rs.getFloat("c");
							Date d = rs.getDate("d");
							if(a != i ) {
								throw new IllegalStateException(String.format("%d != %d",a , i));
							}
							if(!b.equals("test-" + i)) {
								throw new IllegalStateException(String.format("%s != %s",b , "test-" + i));
							}
							if(c!=i * 1.5) {
								throw new IllegalStateException(String.format("%d != %d",c , i * 1.5));
							}
							inst.setTime(d);
							if(inst.get(Calendar.MONTH) != i % 12) {
								throw new IllegalStateException(String.format("%d != %d",inst.get(Calendar.MONTH) , i %12));
							}
						}
					}

				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
