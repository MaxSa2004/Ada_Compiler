procedure main is
begin
    x := 1 + 2 * 3 - (4 / 2) + (5 - (6 - 1));
    if x >= 0 then
        put_line(x);
        put_line("nonneg");
    else
        put_line("neg");
    end if;
end main;

-- x = 5, 5nonneg