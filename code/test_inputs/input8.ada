procedure main is
begin
    i := 0;
    while i < 4 loop
        if i < 2 then
            put_line("low");
        else
            put_line("high");
        end if;
        i := i + 1;
    end loop;
end main;