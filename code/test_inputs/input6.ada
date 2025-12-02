procedure main is
begin
    a := 3;
    b := 5;
    t := (a < b) and not (a = 0) or (b <= 5);
    if t then
        put_line("true");
    else
        put_line("false");
    end if;
end main;