require 'formula'

class Libomron <Formula
  url ''
  homepage 'http://libomron.nonpolynomial.com/'
  md5 ''

  depends_on 'cmake' => :build
  depends_on 'libusb'

  def install
   mkdir 'libomron-build'
    Dir.chdir 'libomron-build' do      
      makefiles = "cmake -G 'Unix Makefiles' -DCMAKE_INSTALL_PREFIX:PATH=#{prefix} .."
      system makefiles
      system "make"
      system "make install"
    end
  end

end
