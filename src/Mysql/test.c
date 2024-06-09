#include <mysql/mysql.h>
#include <stdio.h>

int main()
{

    MYSQL *mysql = mysql_init(NULL);
    if (mysql == NULL)
    {
        printf("mysql init() error\n");
        return -1;
    }
    mysql = mysql_real_connect(mysql, "localhost", "root", "##", "test", 0, NULL, 0);
    if (mysql == NULL)
    {
        printf("mysql readl connect error\n");
        return -1;
    }

    printf("mysql api使用的默认编码: %s\n", mysql_character_set_name(mysql));

    // 设置编码为utf8
    // mysql_set_character_set(mysql, "utf8");

    // printf("设置后的编码: %s\n", mysql_character_set_name(mysql));

    printf("mysql connect successful\n");

    const char *sql = "select * from dept";

    int ret = mysql_query(mysql, sql);
    if (ret != 0)
    {
        printf("mysql query error because %s\n", mysql_error(mysql));
        return -1;
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    if (res == NULL)
    {
        printf("mysql store result error because %s\n", mysql_error(mysql));
        return -1;
    }

    int num = mysql_num_fields(res);

    MYSQL_FIELD *fields = mysql_fetch_fields(res);
    for (int i = 0; i < num; ++i)
    {
        printf("%s\t\t", fields[i].name);
    }
    printf("\n");
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        for (int i = 0; i < num; ++i)
        {
            printf("%s\t\t", row[i]);
        }
        printf("\n");
    }
    mysql_free_result(res);

    mysql_autocommit(mysql, 0);
    int ret1 = mysql_query(mysql, "insert into dept values(13, 'echo', 'school')");
    int ret2 = mysql_query(mysql, "insert into dept values(14, 'bai', 'bed')");
    int ret3 = mysql_query(mysql, "insert into dept values(15, 'echobai', 'lab')");

    printf("ret1 = %d, ret2 = %d, ret3 = %d\n", ret1, ret2, ret3);

    if (!ret1 && !ret2 && !ret3)
    {
        mysql_commit(mysql);
    }
    else
    {
        mysql_rollback(mysql);
    }

    mysql_close(mysql);

    return 0;
}
