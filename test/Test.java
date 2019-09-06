import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class Test {

	public static void main(String[] args) {

		// auto close connection
		try (Connection conn = DriverManager.getConnection("jdbc:postgresql://127.0.0.1:5433/test")) {

			PreparedStatement stmt = conn.prepareStatement("select * from test where a=?");
			stmt.setInt(1, 1);
			ResultSet rs = stmt.executeQuery();

			// STEP 5: Extract data from result set
			while (rs.next()) {
				// Retrieve by column name
				int b = rs.getInt("b");
				float c = rs.getFloat("c");
				String a = rs.getString("a");

				// Display values
				System.out.print("a: " + a);
				System.out.print(", b: " + b);
				System.out.println(", c: " + c);
			}
			
			stmt.setInt(1, 2);
			rs = stmt.executeQuery();

			// STEP 5: Extract data from result set
			while (rs.next()) {
				// Retrieve by column name
				int b = rs.getInt("b");
				float c = rs.getFloat("c");
				String a = rs.getString("a");

				// Display values
				System.out.print("a: " + a);
				System.out.print(", b: " + b);
				System.out.println(", c: " + c);
			}

		} catch (SQLException e) {
			System.err.format("SQL State: %s\n%s", e.getSQLState(), e.getMessage());
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
