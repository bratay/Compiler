set breakpoint pending on
set confirm off
file ./holycc
break lake::Err::report
commands
	where
end
break holyc::ToDoError::ToDoError
commands
	where
end
break holyc::InternalError::InternalError
commands
	where
end

define p2
  set args p2_tests/$arg0.holyc -p --
  run
end
