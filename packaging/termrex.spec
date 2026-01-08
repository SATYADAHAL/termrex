Name:           termrex
Version:        1.0.2
Release:        1%{?dist}
Summary:        Terminal-based endless runner game inspired by Chrome Dino

License:        MIT
URL:            https://github.com/satyadahal/termrex
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc-c++, make

%description
TERM-REX (termrex) is a terminal-based endless runner game inspired by the Chrome Dino offline game.
Jump or duck to avoid obstacles while running endlessly in your terminal.
%prep
%setup -q

%build
make CXXFLAGS="%{optflags}"

%install
rm -rf %{buildroot}
# Install binary to /usr/bin
make DESTDIR=%{buildroot} BINDIR=%{_bindir} install

%files
%license LICENSE
%doc README.md readme-images/
%{_bindir}/termrex

%changelog
* Thu Jan 08 2026 Satya Prakash Dahal <dahal.satya321@gmail.com> - 1.0.2-1
- Initial Fedora COPR packaging
