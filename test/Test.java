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
				for (int i = 0; i < 10; ++i) {
					stmt.setInt(1, i);
					stmt.setString(2, "test-" + i);
					stmt.setDouble(3, i * 1.5);
					Calendar cal = Calendar.getInstance();
					cal.set(Calendar.MONTH, i);
					stmt.setDate(4, new java.sql.Date(cal.getTime().getTime()));
					stmt.execute();
				}
			}
			System.out.println("Insert done");
			try (PreparedStatement stmt = conn.prepareStatement("select * from test where a=?")) {
				for (int i = 0; i < 10; ++i) {
					stmt.setInt(1, i);
					try (ResultSet rs = stmt.executeQuery()) {
						while (rs.next()) {
							// Retrieve by column name
							int a = rs.getInt("a");
							String b = rs.getString("b");
							float c = rs.getFloat("c");
							Date d = rs.getDate("d");
							// Display values
							System.out.print("a: " + a);
							System.out.print(", b: " + b);
							System.out.println(", c: " + c);
							System.out.println(", d: " + d);
						}
					}

				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
