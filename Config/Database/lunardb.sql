USE [LUN-Test-DB]
GO
ALTER TABLE [dbo].[Limits] DROP CONSTRAINT [DF_Limits_TimeStamp]
GO
ALTER TABLE [dbo].[Limits] DROP CONSTRAINT [DF_Limits_ID]
GO
/****** Object:  Table [dbo].[Partnumber]    Script Date: 8/25/2022 1:24:36 PM ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Partnumber]') AND type in (N'U'))
DROP TABLE [dbo].[Partnumber]
GO
/****** Object:  Table [dbo].[Limits]    Script Date: 8/25/2022 1:24:36 PM ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Limits]') AND type in (N'U'))
DROP TABLE [dbo].[Limits]
GO
USE [master]
GO
/****** Object:  Database [LUN-Test-DB]    Script Date: 8/25/2022 1:24:36 PM ******/
DROP DATABASE [LUN-Test-DB]
GO
/****** Object:  Database [LUN-Test-DB]    Script Date: 8/25/2022 1:24:36 PM ******/
CREATE DATABASE [LUN-Test-DB]
 CONTAINMENT = NONE
 ON  PRIMARY 
( NAME = N'LUN-SubModule', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL15.SQLEXPRESS\MSSQL\DATA\LUN-SubModule.mdf' , SIZE = 8192KB , MAXSIZE = UNLIMITED, FILEGROWTH = 65536KB )
 LOG ON 
( NAME = N'LUN-SubModule_log', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL15.SQLEXPRESS\MSSQL\DATA\LUN-SubModule_log.ldf' , SIZE = 8192KB , MAXSIZE = 2048GB , FILEGROWTH = 65536KB )
 WITH CATALOG_COLLATION = DATABASE_DEFAULT
GO
ALTER DATABASE [LUN-Test-DB] SET COMPATIBILITY_LEVEL = 150
GO
IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
EXEC [LUN-Test-DB].[dbo].[sp_fulltext_database] @action = 'enable'
end
GO
ALTER DATABASE [LUN-Test-DB] SET ANSI_NULL_DEFAULT OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET ANSI_NULLS OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET ANSI_PADDING OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET ANSI_WARNINGS OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET ARITHABORT OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET AUTO_CLOSE OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET AUTO_SHRINK OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET AUTO_UPDATE_STATISTICS ON 
GO
ALTER DATABASE [LUN-Test-DB] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET CURSOR_DEFAULT  GLOBAL 
GO
ALTER DATABASE [LUN-Test-DB] SET CONCAT_NULL_YIELDS_NULL OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET NUMERIC_ROUNDABORT OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET QUOTED_IDENTIFIER OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET RECURSIVE_TRIGGERS OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET  DISABLE_BROKER 
GO
ALTER DATABASE [LUN-Test-DB] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET TRUSTWORTHY OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET PARAMETERIZATION SIMPLE 
GO
ALTER DATABASE [LUN-Test-DB] SET READ_COMMITTED_SNAPSHOT OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET HONOR_BROKER_PRIORITY OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET RECOVERY SIMPLE 
GO
ALTER DATABASE [LUN-Test-DB] SET  MULTI_USER 
GO
ALTER DATABASE [LUN-Test-DB] SET PAGE_VERIFY CHECKSUM  
GO
ALTER DATABASE [LUN-Test-DB] SET DB_CHAINING OFF 
GO
ALTER DATABASE [LUN-Test-DB] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO
ALTER DATABASE [LUN-Test-DB] SET TARGET_RECOVERY_TIME = 60 SECONDS 
GO
ALTER DATABASE [LUN-Test-DB] SET DELAYED_DURABILITY = DISABLED 
GO
ALTER DATABASE [LUN-Test-DB] SET ACCELERATED_DATABASE_RECOVERY = OFF  
GO
ALTER DATABASE [LUN-Test-DB] SET QUERY_STORE = OFF
GO
USE [LUN-Test-DB]
GO
/****** Object:  Table [dbo].[Limits]    Script Date: 8/25/2022 1:24:36 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Limits](
	[ID] [uniqueidentifier] NOT NULL,
	[TimeStamp] [datetime] NOT NULL,
	[ErrorCodes] [varchar](50) NOT NULL,
	[LowLimit] [numeric](18, 6) NULL,
	[HighLimit] [numeric](18, 6) NULL,
	[Comparator] [varchar](10) NULL,
	[Units] [varchar](10) NULL,
	[StringComp] [varchar](50) NULL,
	[Models] [varchar](50) NOT NULL,
	[Description] [varchar](50) NULL,
 CONSTRAINT [PK_Limits] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Partnumber]    Script Date: 8/25/2022 1:24:36 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Partnumber](
	[ID] [uniqueidentifier] NOT NULL,
	[PartNumberRegex] [varchar](255) NOT NULL,
	[Model] [varchar](10) NOT NULL,
	[Suffix] [varchar](10) NOT NULL,
 CONSTRAINT [PK_Partnumber] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'a2464b18-a217-4be2-a2b2-0282c8d52d21', CAST(N'2022-02-07T10:37:40.210' AS DateTime), N'20003', CAST(7.125000 AS Numeric(18, 6)), CAST(8.000000 AS Numeric(18, 6)), N'GELT', N'Vdc', N'OCV', N'SubM', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'31b3f1ab-26d9-48fa-bd50-1b5b0bd58541', CAST(N'2021-12-21T14:20:02.430' AS DateTime), N'LimitCode2', CAST(10.000000 AS Numeric(18, 6)), CAST(20.000000 AS Numeric(18, 6)), N'GELE', N'Amps', N' ', N'Rev1,Rev2,Rev3', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'70ae7314-7491-4715-8083-204854cb429d', CAST(N'2022-08-23T02:23:12.730' AS DateTime), N'70007', CAST(0.000000 AS Numeric(18, 6)), CAST(0.500000 AS Numeric(18, 6)), N'LT', N'A', N'1st Solar out metering CT', N'GIP', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'00be67d3-9f08-4f0a-a89f-208c7f5be216', CAST(N'2022-07-31T12:53:09.480' AS DateTime), N'40001', CAST(0.000000 AS Numeric(18, 6)), CAST(5.000000 AS Numeric(18, 6)), N'GELE', N'A', N'Current', N'Nazgul', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'8d5500d5-a3e3-442b-9b6d-2274291d54bb', CAST(N'2022-08-23T02:25:44.057' AS DateTime), N'70008', CAST(0.000000 AS Numeric(18, 6)), CAST(0.500000 AS Numeric(18, 6)), N'LT', N'A', N'2nd Solar out metering CT', N'GIP', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'718c40f2-ae22-4b50-8993-23f540912c7b', CAST(N'2022-02-02T10:02:42.180' AS DateTime), N'10003', CAST(3.500000 AS Numeric(18, 6)), CAST(4.000000 AS Numeric(18, 6)), N'GELT', N'Vdc', N'Vcell2', N'SubM', N'V > 3.5')
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'bf0a7e85-7040-4f1e-a35a-2860aa1840f7', CAST(N'2022-01-28T17:28:05.323' AS DateTime), N'20002', CAST(-180.000000 AS Numeric(18, 6)), CAST(180.000000 AS Numeric(18, 6)), N'GELE', N'Deg', N'Phase Angle', N'SubM', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'37368f2b-3f60-48af-bd5e-35b5e20d0c3a', CAST(N'2022-08-03T10:34:19.210' AS DateTime), N'70002', CAST(12.000000 AS Numeric(18, 6)), CAST(15.000000 AS Numeric(18, 6)), N'GELT', N'VDC', N'AC/DC', N'GIP', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'09e96158-262c-4bf9-a10b-36724e21043f', CAST(N'2022-08-23T02:27:42.967' AS DateTime), N'70010', CAST(216.000000 AS Numeric(18, 6)), CAST(264.000000 AS Numeric(18, 6)), N'GELT', N'VAC', N'2nd Voltage', N'GIP', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'69df2bde-2ef5-4480-9621-53a0bbae9031', CAST(N'2022-02-25T10:27:17.630' AS DateTime), N'30002', CAST(0.000000 AS Numeric(18, 6)), CAST(0.250000 AS Numeric(18, 6)), N'GELE', N'mA', N'HiPOT TP2', N'SubM', N'Current < 0.25')
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'7abd0022-a7f6-4870-b111-61902a9ba75b', CAST(N'2022-08-23T02:22:32.423' AS DateTime), N'70006', CAST(0.000000 AS Numeric(18, 6)), CAST(0.500000 AS Numeric(18, 6)), N'LT', N'A', N'2nd Grid In metering CT ', N'GIP', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'6974b6ec-368b-4157-8250-6ea3170a4750', CAST(N'2021-12-21T14:17:50.983' AS DateTime), N'Sample1', CAST(5.000000 AS Numeric(18, 6)), CAST(15.000000 AS Numeric(18, 6)), N'GTLE', N'Vdc', N' ', N'Rev2', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'a0fc4a2c-9521-4618-80e1-84a6b0989159', CAST(N'2022-08-03T10:34:19.217' AS DateTime), N'70003', CAST(0.000000 AS Numeric(18, 6)), CAST(0.500000 AS Numeric(18, 6)), N'LT', N'A', N'1st Grid out metering CT', N'GIP', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'd9056ba5-9572-432e-8ddb-88d43bb8612b', CAST(N'2022-01-10T13:50:13.290' AS DateTime), N'10002', CAST(3.500000 AS Numeric(18, 6)), CAST(4.000000 AS Numeric(18, 6)), N'GELT', N'Vdc', N'Vcell1', N'SubM', N'V>3.5')
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'b3fe3c6a-c067-473d-a9f5-8f66ab39582e', CAST(N'2021-12-21T14:17:12.400' AS DateTime), N'A0001243', CAST(5.000000 AS Numeric(18, 6)), CAST(10.000000 AS Numeric(18, 6)), N'GELE', N'C', N' ', N'Rev1,Rev3', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'a8172288-b61a-4dfb-8294-9368eff1aec5', CAST(N'2022-01-10T13:55:31.553' AS DateTime), N'30001', CAST(0.000000 AS Numeric(18, 6)), CAST(0.250000 AS Numeric(18, 6)), N'GELE', N'mA', N'HiPOT TP1', N'SubM', N'0<Current < 0.25')
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'3e7965de-1254-475b-bca5-adae56df1c41', CAST(N'2022-01-10T13:49:08.250' AS DateTime), N'10001', CAST(7.125000 AS Numeric(18, 6)), CAST(8.000000 AS Numeric(18, 6)), N'GELT', N'Vdc', N' OCV', N'SubM', N'V> 7.152')
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'c82a170d-78fa-48fb-9c83-b2ab1ba8c0b1', CAST(N'2022-01-10T13:53:06.643' AS DateTime), N'20001', CAST(0.000000 AS Numeric(18, 6)), CAST(0.006000 AS Numeric(18, 6)), N'GTLT', N'ohm', N'Impedence(Z)', N'SubM', N'0 < Resist < 0.00413')
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'8d2ddc06-b2fa-400c-a9ed-b52249ee1500', CAST(N'2022-08-23T02:20:59.430' AS DateTime), N'70005', CAST(0.000000 AS Numeric(18, 6)), CAST(0.500000 AS Numeric(18, 6)), N'LT', N'A', N'2nd Grid out metering CT', N'GIP', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'7fcc4ead-ebc2-41b7-9084-b5d14eafeeee', CAST(N'2021-12-21T14:20:30.533' AS DateTime), N'A0001243', CAST(10.000000 AS Numeric(18, 6)), CAST(20.000000 AS Numeric(18, 6)), N'GELE', N'C', N' ', N'Rev3,Rev2', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'22f6fcc3-04e0-43a7-95a2-b929e4666d8c', CAST(N'2022-07-13T13:21:41.887' AS DateTime), N'10004', CAST(-100.000000 AS Numeric(18, 6)), CAST(1000.000000 AS Numeric(18, 6)), N'GELE', N'DegC', N'Thermistor', N'SubM', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'2d4e0c3f-eff1-4457-8c06-be16f759febf', CAST(N'2022-08-23T02:26:11.427' AS DateTime), N'70009', CAST(216.000000 AS Numeric(18, 6)), CAST(264.000000 AS Numeric(18, 6)), N'GELT', N'VAC', N'1st Voltage', N'GIP', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'b850d529-c59e-4f7c-a7e6-c034d49ac7ba', CAST(N'2022-08-23T02:19:05.950' AS DateTime), N'70004', CAST(0.000000 AS Numeric(18, 6)), CAST(0.500000 AS Numeric(18, 6)), N'LT', N'A', N'1st Grid In metering CT', N'GIP', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'ad0af5c2-2fe7-4ff1-a8e1-c0c5faf8b20d', CAST(N'2021-12-21T14:57:14.250' AS DateTime), N'A0001243', CAST(5.000000 AS Numeric(18, 6)), CAST(10.000000 AS Numeric(18, 6)), N'GELE', N'C', N' ', N'Rev3', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'2d068873-702a-4d6d-8183-cf1deaa4aaad', CAST(N'2022-07-31T12:54:24.180' AS DateTime), N'40002', CAST(-200.000000 AS Numeric(18, 6)), CAST(200.000000 AS Numeric(18, 6)), N'GELE', N'VDC', N'Voltage', N'Nazgul', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'dba5c472-6efc-4536-816e-dc3243966d5a', CAST(N'2022-08-03T10:23:57.240' AS DateTime), N'70001', CAST(3.100000 AS Numeric(18, 6)), CAST(3.500000 AS Numeric(18, 6)), N'GELT', N'VDC', N'Power up Voltage', N'GIP', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'2b6d9721-a207-495d-bdcd-e6f4f1a74b11', CAST(N'2022-07-31T12:55:10.977' AS DateTime), N'40003', CAST(-40.000000 AS Numeric(18, 6)), CAST(100.000000 AS Numeric(18, 6)), N'GELE', N'DegC', N'Temperature', N'Nazgul', NULL)
GO
INSERT [dbo].[Partnumber] ([ID], [PartNumberRegex], [Model], [Suffix]) VALUES (N'fa4f72a4-5e99-4093-956d-40a0ba740e36', N'^(P:)(\w{2}-\w{6})(R:)(\w{2})(S:)((\w{3})(\d{2})(\d{2})(\d{3}))$', N'SubM', N'A3')
INSERT [dbo].[Partnumber] ([ID], [PartNumberRegex], [Model], [Suffix]) VALUES (N'54d08a76-3c8c-49c7-9978-941d39cdbd69', N'^(P:)(\w{2}-\w{6})(R:)(\w{2})(S:)((\w{3})(\d{2})(\d{2})(\d{3}))$', N'GIP', N'A2')
INSERT [dbo].[Partnumber] ([ID], [PartNumberRegex], [Model], [Suffix]) VALUES (N'a382ead5-775e-4515-9346-9721282186ab', N'^(P:)(\w{2}-\w{6})(R:)(\w{2})(S:)((\w{3})(\d{2})(\d{2})(\d{3}))$', N'PB', N'A1')
INSERT [dbo].[Partnumber] ([ID], [PartNumberRegex], [Model], [Suffix]) VALUES (N'f7082cff-7ec1-4581-896a-b08b3c7ed821', N'^(P:)(\w{2}-\w{6})(R:)(\w{2})(S:)((\w{3})(\d{2})(\d{2})(\d{3}))$', N'Nazgul', N'A4')
GO
ALTER TABLE [dbo].[Limits] ADD  CONSTRAINT [DF_Limits_ID]  DEFAULT (newid()) FOR [ID]
GO
ALTER TABLE [dbo].[Limits] ADD  CONSTRAINT [DF_Limits_TimeStamp]  DEFAULT (getdate()) FOR [TimeStamp]
GO
USE [master]
GO
ALTER DATABASE [LUN-Test-DB] SET  READ_WRITE 
GO
