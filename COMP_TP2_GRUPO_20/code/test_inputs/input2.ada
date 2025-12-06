procedure Main is
begin
   -- Input example
   get_line(name);

   -- Assignment and arithmetic expressions
   x := -5;
   y := 10;
   -- z = -50 + (-4) = -54
   z := x * y + (x - 3) / 2; 

   -- Boolean expressions
   flag := true;
   -- condition = ((1 and !0) or 0) = 1
   condition := (x < y) and not(flag = false) or (z >= 20);

   -- Conditional (if-then-else)
   if condition then
      put_line("Condition is true!");
   else
      put_line("Condition is false!");
   end if;

   -- While loop example
   while x < 5 loop
      x := x + 1;
      put_line("Incrementing X...");
   end loop;

   -- Another output
   put_line("Final value of X:");
   put_line(x);
   put_line("from:");
   put_line(name);
end main;
