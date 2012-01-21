#!/usr/bin/env ruby

require 'fileutils'

resources = []
dst = ARGV[1]
fdst = File.new(dst, 'w')

fdst.puts "#ifndef nui_resources_h"
fdst.puts "#define nui_resources_h"
fdst.puts ""
fdst.puts "namespace nui {"
fdst.puts "    typedef struct _Resource {"
fdst.puts "        const char *name;"
fdst.puts "        const unsigned char *data;"
fdst.puts "        size_t length;"
fdst.puts "    } Resource;"
fdst.puts ""

Dir.glob(File.join(ARGV[0], '*')) { |src|
	name = File.basename(src)
	symb = "data_" + name.sub(/[\.]/, '_')
	fsrc = File.new(src)
	count = 0
	
	begin
		fdst.print "    const unsigned char #{symb}[] = { ";
		
		fsrc.each_byte { |byte|
			fdst.print "," if count > 0
			fdst.print "0x#{byte.to_s(16)}";
			count += 1
		}
		
		fdst.puts " };"
		
		resources.push [ name, symb ]
	rescue
		puts "Copying #{src} failed!"
	end
	
	fsrc.close
}

fdst.puts ""
fdst.puts "    static const Resource _resources[] = {"
resources.each { |resource| fdst.puts "        { \"#{resource[0]}\", #{resource[1]}, sizeof(#{resource[1]}) }," }
fdst.puts "        { NULL, NULL, 0 }"
fdst.puts "    };"
fdst.puts "}"
fdst.puts ""
fdst.puts "#endif"
fdst.close
